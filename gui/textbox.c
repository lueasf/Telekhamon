#include "textbox.h"

#include <stdlib.h>





// Met à k=jour les rects de la texture de box.
void textBoxUpdateTexture(TextBox* box) {

    // Remet à zéro la position des rects.
    box->image_rect.x = 0;
    box->image_rect.y = 0;
    box->render_rect.x = 0;
    box->render_rect.y = 0;

    // Récupère les dimensions de la texture.
    if (box->text_texture) SDL_QueryTexture(box->text_texture, NULL, NULL, &box->image_rect.w, &box->image_rect.h);
    else {
        box->image_rect.w = 0;
        box->image_rect.h = 1;
    }

    // Calcule la taille à l'écran en fonction de la hauteur de la boite.
    float render_ratio = box->rect.h/(float)box->image_rect.h;
    box->render_rect.w = box->image_rect.w*render_ratio;
    box->render_rect.h = box->rect.h;

    // Récupère la position du curseur.
    int cursor_position;
    TTF_SizeText(box->font, box->text, &box->cursor_position, NULL);
    TTF_SizeText(box->font, box->text+box->cursor, &cursor_position, NULL);
    box->cursor_position -= cursor_position;
    box->cursor_position = box->rect.x + box->cursor_position*(box->render_rect.h/(float)box->image_rect.h);

    // Modifie l'offset pour forcer le curseur à rester dans la boite.
    if (box->cursor_position - box->offset > box->rect.x+box->rect.w-4)
        box->offset = box->cursor_position - (box->rect.x+box->rect.w-4);
    if (box->cursor_position - box->offset < box->rect.x+4)
        box->offset = box->cursor_position - (box->rect.x+4);

    // Modifie la position du curseur et du rect en conséquent.
    box->cursor_position -= box->offset;
    box->render_rect.x = box->rect.x-box->offset;
    box->render_rect.y = box->rect.y+4;

    // Redimensionne les rects pour éviter les dépassements à gauche.
    if (box->render_rect.x < box->rect.x+2) {
        float ratio = (box->rect.x+2-box->render_rect.x)/(float)box->render_rect.w;
        box->render_rect.x = box->rect.x;
        box->render_rect.w *= 1.0f-ratio;
        box->image_rect.x = box->image_rect.w*ratio;
        box->image_rect.w *= 1.0f-ratio;
    }

    // Redimensionne les rects pour éviter les dépassements à droite.
    if (box->render_rect.x+box->render_rect.w > box->rect.x+box->rect.w-2) {
        float ratio = (box->render_rect.x+box->render_rect.w-(box->rect.x+box->rect.w-2))/(float)box->render_rect.w;
        box->render_rect.w *= 1.0f-ratio;
        box->image_rect.w *= 1.0f-ratio;
    }

}





// Fonction d'insertion d'un caractère dans un string.
// Insère c dans string de longueur *size à id et incrémente *size.
// Hyp : le buffer qui contient la chaine n'est pas plein, 0 <= id <= size.
void stringInsert(char* string, unsigned int id, unsigned int* size, char c) {

    unsigned int n = *size;
    while (n != id) {
        string[n] = string[n-1];
        n--;
    }
    string[id] = c;
    (*size)++;
}



// Fonction de suppression d'un caractère dans un string.
// Supprime dans string le caractère à id et décrémente *size.
// Hyp : 0 <= id < size.
void stringErase(char* string, unsigned int id, unsigned int* size) {
    while (string[id] != '\0') {
        string[id] = string[id+1];
        id++;
    }
    (*size)--;
}





// Constructeur de TextBox.
TextBox* createTextBox(TTF_Font* font, unsigned int buffer_size, int default_state) {

    // Malloc la box.
    TextBox* box = (TextBox*)malloc(sizeof(TextBox));
    if (!box) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du malloc de createTextBox");
        abort();
    }

    // Initialise la box.
    box->font = font;
    box->text_texture = NULL;
    box->offset = 0;
    box->state = default_state;
    box->cursor = 0;
    box->cursor_position = 0;
    box->buffer_size = buffer_size;
    box->text_size = 0;

    // Malloc le buffer pour le texte.
    box->text = (char*)calloc(buffer_size+1, sizeof(char));
    if (!box->text) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du malloc de createTextBox");
        abort();
    }

    // Renvoie la box initialisée.
    return box;

}



// Destructeur de TextBox.
void freeTextBox(TextBox* box) {
    if (box->text) free(box->text);
    if (box->text_texture) SDL_DestroyTexture(box->text_texture);
    free(box);
}





// Déplace le curseur vers la gauche.
void textBoxLeft(TextBox* box) {
    if (!box->state) return;
    if (box->cursor > 0) {

        // Si ctrl, saute d'un mot, d'un caractère sinon.
        SDL_Keymod mod = SDL_GetModState();
        if (mod & KMOD_CTRL) {
            do {
                box->cursor--;
            } while (box->cursor > 0 && box->text[box->cursor-1] != ' ');
        } else box->cursor--;
        
        // Met à jour les rects de la texture.
        textBoxUpdateTexture(box);

    }
}



// Déplace le curseur vers la gauche.
void textBoxRight(TextBox* box) {
    if (!box->state) return;
    if (box->cursor < box->text_size) {

        // Si ctrl, saute d'un mot, d'un caractère sinon.
        SDL_Keymod mod = SDL_GetModState();
        if (mod & KMOD_CTRL) {
            while (box->cursor < box->text_size && box->text[box->cursor] != ' ') box->cursor++;
            if (box->text[box->cursor] == ' ') box->cursor++;
        } else box->cursor++;

        // Met à jour les rects de la texture.
        textBoxUpdateTexture(box);

    }
}



// Gestion du clic gauche.
void textBoxLeftClick(TextBox* box, int x, int y) {
    box->state = (
        x >= box->rect.x && y >= box->rect.y && 
        x <= box->rect.x+box->rect.w && y <= box->rect.y+box->rect.h
    );
}



// Gestion des entrées de texte au clavier.
void textBoxProcessEvent(TextBox* box, SDL_Renderer* renderer, SDL_Keycode code) {
    if (!box->state) return;
    
    // Vérifie si la touche représente un caractère affichable.
    SDL_Keymod mod = SDL_GetModState();
    int last_cursor = box->cursor;
    if ((code >= '0' && code <= '9') || (code >= 'a' && code <= 'z') ||
        code == '_' || code == '-' || code == ' ') {

        // Evite les buffer overflow.
        if (box->text_size >= box->buffer_size) return;
            
        // Ajoute les caractères des touches numériques.
        if ((code >= '0' && code <= '9')) {
            if (mod & (KMOD_SHIFT|KMOD_CAPS)) stringInsert(box->text, box->cursor, &box->text_size, code);
            else if (code == '6') stringInsert(box->text, box->cursor, &box->text_size, '-');
            else if (code == '8') stringInsert(box->text, box->cursor, &box->text_size, '_');
            else return;
            
        // Ajoute les lettres.
        } else {
            if ((mod & (KMOD_SHIFT|KMOD_CAPS)) && code >= 'a' && code <= 'z') stringInsert(box->text, box->cursor, &box->text_size, 'A'+code-'a');
            else stringInsert(box->text, box->cursor, &box->text_size, code);
        }

        // Met à jour la position du curseur.
        box->cursor++;
        
    }
    
    // Supression au besoin d'un caractère.
    if (code == SDLK_DELETE && box->cursor < box->text_size) {
        stringErase(box->text, box->cursor, &box->text_size);
    } else if (code == SDLK_BACKSPACE && box->cursor > 0) {
        if (mod & KMOD_CTRL) {
        
            // Supprime tout un mot.
            int space = 0;
            while (box->cursor > 0 && (!space || box->text[box->cursor-1] == ' ')) {
                if (box->text[box->cursor-1] == ' ') space = 1;
                box->cursor--;
                stringErase(box->text, box->cursor, &box->text_size);
            }

        // Supprime un caractère.
        } else {
            box->cursor--;
            stringErase(box->text, box->cursor, &box->text_size);
        }
    }

    // Supprime l'ancienne texture.
    if (box->text_texture) SDL_DestroyTexture(box->text_texture);
    box->text_texture = NULL;  

    // Crée la nouvelle texture.
    SDL_Color foreground_color = {0x00,0x00,0x00,0xFF};
    SDL_Surface* surface = TTF_RenderText_Blended(box->font, box->text, foreground_color);
    if (surface) {
        box->text_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    // Met à jour les rects de la texture.
    textBoxUpdateTexture(box);

}





// Fonction de redimensionnement de la box.
void textBoxResize(TextBox* box, SDL_Rect* rect) {

    // Enregistre les nouvelle dimensions.
    box->rect.x = rect->x;
    box->rect.y = rect->y;
    box->rect.w = rect->w;
    box->rect.h = rect->h;    

    // Met à jour les rects de la texture.
    textBoxUpdateTexture(box);

}
