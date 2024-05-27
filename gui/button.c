#include "button.h"

#include <stdlib.h>
#include <SDL2/SDL_image.h>

#include "../graphics/font.h"





// Constructeur de Button.
Button* createButton(SDL_Renderer* renderer, TTF_Font* font, char* path_in, char* path_out, float area[8], char* text,
    void** args, void (*func)(void**)) {
    
    Button* button = (Button*)malloc(sizeof(Button));
    if (!button) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Button");
        abort();
    }

    // Charge la texture du bouton pressé.
    button->texture_in = IMG_LoadTexture(renderer, path_in);
    if (!button->texture_in) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }

    // Charge la texture du bouton.
    button->texture_out = IMG_LoadTexture(renderer, path_out);
    if (!button->texture_out) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }

    // Crée le texte du bouton au besoin.
    SDL_Color color = {0x00,0x00,0x00,0xFF};
    if (text) button->text = createTextureFromText(renderer, font, &color, text);
    else button->text = NULL;

    // Récupère la taille du bouton.
    SDL_QueryTexture(button->texture_in, NULL, NULL, &button->rect.w, &button->rect.h);
    button->rect.x = 300;
    button->rect.y = 0;

    // Enregistre la zone d'appui.
    for (unsigned int i = 0; i < 8; i++) button->area[i] = area[i];

    // Enregistre la fonction d'action et son argument.
    button->args = args;
    button->func = func;

    // Renvoie le bouton initialisé.
    return button;

}



// Destructeur de Button. 
void freeButton(Button* button) {
    if (button->texture_in) SDL_DestroyTexture(button->texture_in);
    if (button->texture_out) SDL_DestroyTexture(button->texture_out);
    if (button->text) SDL_DestroyTexture(button->text);
    if (button->args) free(button->args);
    free(button);
}





// Permet de savoir si le point (x,y) est dans la zone de click du bouton.
int inButton(Button* button, int x, int y) {
    
    // Calcule les sommets du bouton.
    int points[8];
    for (unsigned int i = 0; i < 4; i++) {
        points[2*i] =   button->rect.x + button->rect.w*button->area[2*i];
        points[2*i+1] = button->rect.y + button->rect.h*button->area[2*i+1];
    }

    // Vérifie la présence du point dans le bouton.
    for (unsigned int i = 0; i < 4; i++) {
        int A[2] = {points[2*i] - x, points[2*i+1] - y};
        int B[2] = {points[2*((i+1)%4)] - points[2*i], points[2*((i+1)%4)+1] - points[2*i+1]};
        if ((A[0])*(B[1]) - (A[1])*(B[0]) < 0) return 0;       
    }

    // Renvoie true s'il y est.
    return 1;

}



// Exécute l'action du bouton s'il est cliqué.
void onClick(Button* button, int x, int y) {
    if (inButton(button, x, y) && button->func) button->func(button->args);
}
