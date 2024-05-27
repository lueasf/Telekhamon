#pragma once

#include <SDL2/SDL.h>





// Structure pour modéliser un slider.
struct s_Slider {
    SDL_Rect rect;
    float value;
    int locked;
};
typedef struct s_Slider Slider;





// Constructeur, destructeur.
Slider* createSlider(float value);
void freeSlider(Slider* slider);




// Fonctions d'update.
int sliderUpdate(Slider* slider, unsigned int m_x);
void sliderLeftClick(Slider* slider, unsigned int m_x, unsigned int m_y);
void sliderLeftClickReleased(Slider* slider);




// Fonction de rediemsionnement.
void sliderResize(Slider* slider, SDL_Rect* rect);
