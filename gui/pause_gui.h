#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../game_state.h"
#include "button.h"
#include "slider.h"





// Enum des états du menu pause.
typedef enum {
    PGS_PAUSE,
    PGS_OPTIONS
} PauseGUIState;



// Structure pour modéliser le menu de pause.
struct s_PauseGUI {
    unsigned int w;
    unsigned int h;
    unsigned int m_x;
    unsigned int m_y;
    SDL_Texture* pause_label;
    SDL_Texture* volume_label;
    SDL_Rect button_rect;
    Button* buttons[4];
    PauseGUIState state;
    Slider* volume;
    Slider* main_volume;
};
typedef struct s_PauseGUI PauseGUI;





// Constructeur, destructeur.
PauseGUI* createPauseGUI(SDL_Renderer* renderer, TTF_Font* font, unsigned int w, unsigned int h,
    int* pause, GameState* game_state, int* mm_state);
void freePauseGUI(PauseGUI* pause_gui);



// Fonctions d'update du menu.
void pauseGUIUpdate(PauseGUI* gui, unsigned int m_x, unsigned int m_y);
void pauseGUILeftClick(PauseGUI* gui);
void pauseGUILeftClickDown(PauseGUI* gui);



// Fonction de redimensionnement.
void pauseGUISetScreenSize(PauseGUI* gui, unsigned int w, unsigned int h);
