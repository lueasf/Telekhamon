#include "font.h"

#include <ctype.h>
#include <string.h>





// Constructeur de TTF_Font.
// path : chemin vers le fichier .ttf à utiliser.
// size : taille de police à utiliser. Attention ne peut pas être changée.
TTF_Font* createFont(char* path, int size) {
    TTF_Font* font = TTF_OpenFont(path, size);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement d'une police : %s", SDL_GetError());
        abort();
    }
    return font;
}



// Destructeur de TTF_Font.
void freeFont(TTF_Font* font) {
    if (!font) return;
    TTF_CloseFont(font);
}





// Permet de créer une texture à partir d'un texte.
// color : la couleur du texte.
// text : le texte à rendre.
SDL_Texture* createTextureFromText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color, char* text) {

    // Rend le texte dans une surface.
    SDL_Color background = {0,0,0,0};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, *color);
    if (!surface) goto TEXTURE_CREATION_ERROR;

    // Crée une texture depuis la surface.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) goto TEXTURE_CREATION_ERROR;

    // Renvoie la texture créée.
    SDL_FreeSurface(surface);
    return texture;

    // Gestion des erreures.
TEXTURE_CREATION_ERROR:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du rendu du texte : %s", SDL_GetError());
    abort();

}



// Permet de créer une texture à partir d'un texte avec un outline.
// color : la couleur du texte.
// out_color : la couleur de l'outline.
// out_size : la taille de l'outline.
// text : le texte à rendre.
SDL_Texture* createTextureFromTextOutline(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color, 
    SDL_Color* out_color, int out_size, char* text) {
    SDL_Color background = {0,0,0,0};
    
    // Crée la surface du background.
    SDL_Surface *fg_surface = TTF_RenderText_Blended(font, text, *color);
    if (!fg_surface) goto TEXTURE_CREATION_ERROR;

    // Crée la surface du foreground.
    TTF_SetFontOutline(font, out_size);
    SDL_Surface *bg_surface = TTF_RenderText_Blended(font, text, *out_color);
    TTF_SetFontOutline(font, 0);
    if (!bg_surface) goto TEXTURE_CREATION_ERROR;

    // Blit les deux surfaces ensembles.
    SDL_Rect rect = {0, 0, fg_surface->w, fg_surface->h}; 
    SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND); 
    SDL_SetSurfaceBlendMode(bg_surface, SDL_BLENDMODE_BLEND); 
    SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect);

    // Crée une texture depuis la surface.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
    if (!texture) goto TEXTURE_CREATION_ERROR;

    // Renvoie la texture créée.
    SDL_FreeSurface(bg_surface);
    SDL_FreeSurface(fg_surface);
    return texture;

    // Gestion des erreures.
TEXTURE_CREATION_ERROR:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du rendu du texte : %s", SDL_GetError());
    abort();

}



// Permet de créer une texture à partir d'un texte formaté.
// \ehhhhhh avec hhhhhh un nombre hexa à 6 chiffres permet de changer de couleur.
// color : la couleur du texte.
// text : le texte à rendre.
SDL_Texture* createTextureFromTextFromated(SDL_Renderer* renderer, TTF_Font* font, const char* text) {
    
    // Indice de section de texte.
    unsigned int begin = 0;
    unsigned int end = 0;

    // Coordonnées pour le prochain blit de surface.
    unsigned int x = 0;
    unsigned int y = 0;

    // Variables pour le retour à la ligne.
    unsigned int new_line = 0;
    unsigned int apply_new_line = 0;
    unsigned int end_string = 0;
    
    // Couleur de police et surface de rendu.
    SDL_Color color = {0,0,0,255};
    SDL_Surface* surface = NULL;
    do {
        if (end_string) break;
        
        // Recherche une section de texte à rendre.
        int search = 1;
        while (search) {
            
            // Recherche une fin de section ou un formatage.
            char c, d;
            switch (text[end]) {

            // Fin de section.
            case '\n':
                new_line = 1;
                end++;
                search = 0;
                break;
            case '\0':
                end_string = 1;
                end++;
                search = 0;
                break;

            // Formatage.
            case '\e':
                if (begin == end) {

                    // Vérifie le formatage.
                    for (unsigned int i = 1; i < 7; i++) {
                        c = tolower(text[end+i]);
                        if ((c < 'a' || c > 'f') && (c < '0' || c > '9')) {
                            SDL_LogError(
                                SDL_LOG_CATEGORY_APPLICATION, 
                                "Erreur lors du rendu du texte : mauvais formatage"
                            );
                            abort();
                        }
                    }

                    // Récupère la couleur.
                    c = tolower(text[end+1]); d = tolower(text[end+2]);
                    color.r = 16*((c < 'a') ? c-'0' : c-'a'+10) + ((d < 'a') ? d-'0' : d-'a'+10);
                    c = tolower(text[end+3]); d = tolower(text[end+4]);
                    color.g = 16*((c < 'a') ? c-'0' : c-'a'+10) + ((d < 'a') ? d-'0' : d-'a'+10);
                    c = tolower(text[end+5]); d = tolower(text[end+6]);
                    color.b = 16*((c < 'a') ? c-'0' : c-'a'+10) + ((d < 'a') ? d-'0' : d-'a'+10);

                    // Saute le formatage.
                    begin += 7;
                    end += 7;

                // Si une section à commencé, alors le formatage termine celle-ci.
                } else search = 0;
                break; 

            // Si on à un autre caractère, avance.
            default:
                end++;
            }
            
        }

        // Copie la section dans un buffer.
        char buffer[128];
        strncpy(buffer, text+begin, end-begin+1-2*new_line);
        buffer[end-begin] = 0;
        buffer[end-begin-1] = 0;
        
        // Rend le texte dans une surface.
        SDL_Color background = {0,0,0,0};
        SDL_Rect position = {0,0,0,0};
        SDL_Surface* text = TTF_RenderText_Blended(font, buffer, color);
        if (!text) goto TEXTURE_CREATION_ERROR;
        
        // Si c'est la première surface, la déplace simplement.
        if (!surface) {
            surface = text;
            x = surface->w;

        // Sinon effectue une fusion de surfaces.
        } else {
            SDL_Surface* temp;
            
            // Fusionne vers le bas en cas de nouvelle ligne.
            if (apply_new_line) {
                temp = SDL_CreateRGBSurface(
                    0, fmax(surface->w, text->w), surface->h+text->h, 
                    32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
                );
                if (!temp) goto TEXTURE_CREATION_ERROR;
                y = surface->h;
                x = 0;
                
            // Fusionne vers la droite sinon.
            } else {
                temp = SDL_CreateRGBSurface(0, fmax(surface->w, x+text->w), surface->h, 32,0xFF000000,0x00FF0000,0x0000FF00,0x000000FF);
                if (!temp) goto TEXTURE_CREATION_ERROR;
            }

            // Copie les deux surfaces pour les fusionner.
            SDL_BlitSurface(surface, NULL, temp, NULL);
            position.x = x;
            position.y = y;
            position.w = text->w;
            position.h = text->h;
            SDL_BlitSurface(text, NULL, temp, &position);

            // Supprime les deux surfaces et conserve la surface temparaire comme nouvelle surface.
            x += text->w;
            SDL_FreeSurface(text);
            SDL_FreeSurface(surface);
            surface = temp;

        }

        // Enregistre le retour à la ligne.
        apply_new_line = new_line;
        new_line = 0;

    // Passe à la section suivante si on a pas terminé.
        begin = end;
    } while (text[end] != '\0');
    
    // Crée une texture depuis la surface.
    if (!surface) return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) goto TEXTURE_CREATION_ERROR;

    // Renvoie la texture créée.
    SDL_FreeSurface(surface);
    return texture;

TEXTURE_CREATION_ERROR:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du rendu du texte : %s", SDL_GetError());
    abort();

}



// Permet de créer une texture à partir d'un texte en rajoutant des retours à la ligne.
// hyp : text ne contient pas de retour à la ligne.
// color : la couleur du texte
// text : le texte à rendre.
// max_line_size : la taille maximale en caractères d'une ligne.
SDL_Texture* createTextureFromTextNewLine(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color,
    char* text, int max_line_size) {
    
    // Récupère la hauteur d'une ligne.
    int height;
    TTF_SizeText(font, "A",NULL, &height);

    // Pointeurs sur le texte.
    int begin = 0;
    int end = 0;
    int last_space = 0;

    // Surface et offset pour les retours à la ligne.
    int height_offset = 0;
    SDL_Surface* surface = SDL_CreateRGBSurface(
        0, 1, 1, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
    );

    // Parcours du texte.
    do {

        // Avance end jusqu'à la fin de la ligne.
        while (text[end] != '\0' && text[end] != '\n' && end-begin <= max_line_size) {
            if (text[end] == ' ') last_space = end;
            end++;
        }

        // Calcul l'offset pour les sauts de ligne.
        height_offset = (text[end] == '\n') ? height : 0;

        // Recule end si le dernier mot est trop long.
        if (end-begin > max_line_size) end = last_space;
        
        // Coupe ce dernier mot si c'était le seul sur la ligne. 
        if (end-begin <= 0) end = begin+max_line_size;

        // Récupère le texture de la ligne.
        char buffer[128];
        strncpy(buffer, text+begin, end-begin);
        buffer[end-begin] = 0;

        // Rend ledit texte.
        SDL_Surface* text_surface = TTF_RenderText_Blended(font, buffer, *color);
        if (!text_surface) goto TEXTURE_CREATION_ERROR;

        // Crée la nouvelle surface.
        SDL_Surface* temp = SDL_CreateRGBSurface(
            0, fmax(surface->w, text_surface->w), surface->h+text_surface->h+height_offset, 
            32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
        );
        if (!temp) goto TEXTURE_CREATION_ERROR;

        // Glit l'ancienne surface avec le nouveau texte sur celle-ci.
        SDL_BlitSurface(surface, NULL, temp, NULL);
        SDL_Rect rect = {0, surface->h, text_surface->w, text_surface->h};
        SDL_BlitSurface(text_surface, NULL, temp, &rect);

        // Vide les anciennes surfaces.
        SDL_FreeSurface(surface);
        SDL_FreeSurface(text_surface);
        surface = temp;

        // Repositionne begin et end pour la prochaine ligne.
        // Saute entre autre l'espace qui suit le dernier mot de la ligne.
        begin = end;
        if (text[begin] != '\0') {
            begin++;
            end = begin;
        }

    // Boucle tant qu'on est pas à la fin du string.
    } while (text[begin] != '\0');

    // Convertit la surface en texture.
    if (!surface) return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) goto TEXTURE_CREATION_ERROR;
    return texture;

// Gestion du cas d'erreur.
TEXTURE_CREATION_ERROR:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du rendu du texte : %s", SDL_GetError());
    abort();

}






// Permet de set l'état gras. 
void fontSetBold(TTF_Font* font, int state) {
    int style = TTF_GetFontStyle(font);
    if (state) style |= TTF_STYLE_BOLD;
    else style &= ~TTF_STYLE_BOLD;
    TTF_SetFontStyle(font, style);
}



// Permet de set l'état italique.
void fontSetItalic(TTF_Font* font, int state) {
    int style = TTF_GetFontStyle(font);
    if (state) style |= TTF_STYLE_ITALIC;
    else style &= ~TTF_STYLE_ITALIC;
    TTF_SetFontStyle(font, style);
}



// Permet de set l'état souligné.
void fontSetUnderline(TTF_Font* font, int state) {
    int style = TTF_GetFontStyle(font);
    if (state) style |= TTF_STYLE_UNDERLINE;
    else style &= ~TTF_STYLE_UNDERLINE;
    TTF_SetFontStyle(font, style);
}



// Permet de set l'état barré.
void fontSetStrikethrough(TTF_Font* font, int state) {
    int style = TTF_GetFontStyle(font);
    if (state) style |= TTF_STYLE_STRIKETHROUGH;
    else style &= ~TTF_STYLE_STRIKETHROUGH;
    TTF_SetFontStyle(font, style);
}
