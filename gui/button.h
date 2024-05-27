#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>





// Structure pour modéliser un bouton et ses effets.
// texture_in, texture_out : texture en fonction de si le curseur est sur le bouton ou non.
// rect : taille du bouton.
// area : zone clicable du bouton.
// args, func : fonction et son argument pour gérer les effets d'un bouton.
struct s_Button {
    SDL_Texture* texture_in;
    SDL_Texture* texture_out;
    SDL_Texture* text;
    SDL_Rect rect;
    float area[8];
    void** args;
    void (*func)(void**);
};
typedef struct s_Button Button;



// Constructeur, destructeur.
Button* createButton(SDL_Renderer* renderer, TTF_Font* font, char* path_in, char* path_out, float area[8], char* text,
    void** args, void (*func)(void**));
void freeButton(Button* button);



// Fonctions de gestion du clic.
int inButton(Button* button, int x, int y);
void onClick(Button* button, int x, int y);
