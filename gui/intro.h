#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>





// Structure pour une intro à la Star Wars. 
struct s_Intro {
    unsigned int w;
    unsigned int h;
    int tex_w;
    int tex_h;
    int img_h;
    SDL_Texture* texture;
    SDL_Rect image_rect;
    SDL_Rect render_rect;
    int offset;
};
typedef struct s_Intro Intro;





// Constructeur, destructeur.
Intro* createIntro(SDL_Renderer* renderer, TTF_Font* font, unsigned int w, unsigned int h);
void freeIntro(Intro* intro);



// Fonctions d'update.
void introUpdate(Intro* intro);
int introIsDone(Intro* intro);



// Fonction de redimensionnement.
void introSetStreenSize(Intro* intro, unsigned int w, unsigned int h);
void introUpdateRects(Intro* intro);
