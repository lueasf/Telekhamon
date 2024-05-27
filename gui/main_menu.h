#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#include "button.h"
#include "textbox.h"
#include "slider.h"
#include "../game_state.h"
#include "../world/map.h"
#include "../Fight/player.h"
#include "../entities/level_entity.h"
#include "../graphics/camera.h"
#include "../world/mapgen.h"





// Enum des états du menu.
typedef enum {
    MMS_MAIN,
    MMS_OPTIONS,
    MMS_NEW_RUN,
    MMS_CREDITS
} MainMenuState;





// Nombre de boutons dans l'interface.
#define MM_NUM_BUTTONS 10

// Structure pour le menu principal.
// w,h : dimensions de la fenètre.
// m_x,m_h : position de la souris.
// main_label : titre du jeu.
// background : arrière plan de l'interface.
// difficulty_label,level_number_label,credits : labels du menu.
// font : police pour les rendus.
// background_rect : rect du fond.
// buttton_rect : rect des boutons.
// buttons : boutons du menu.
// state : état du menu.
// box : textbox pour la création de run.
// level_number,difficulty : paramètres pour créer une run.
struct s_MainMenu {
    unsigned int w;
    unsigned int h;
    unsigned int m_x;
    unsigned int m_y;
    SDL_Texture* main_label;
    SDL_Texture* background;
    SDL_Texture* volume_label;
    SDL_Texture* difficulty_label;
    SDL_Texture* level_number_label;
    SDL_Texture* credits;
    TTF_Font* font;
    SDL_Rect background_rect;
    SDL_Rect button_rect;
    Button* buttons[MM_NUM_BUTTONS];
    MainMenuState state;
    TextBox* box;
    Slider* volume;
    int level_number;
    int difficulty;
};
typedef struct s_MainMenu MainMenu;





// Constructeur, destructeur.
MainMenu* createMainMenu(SDL_Renderer* renderer, TTF_Font* font, unsigned int w, unsigned int h,
    int* run, void** argsStartRun);
void freeMainMenu(MainMenu* pause_gui);



// Fonctions d'update du menu.
void mainMenuProcessEvent(MainMenu* gui, SDL_Renderer* renderer, SDL_Keycode code);
void mainMenuUpdate(SDL_Renderer* renderer, MainMenu* gui, unsigned int m_x, unsigned int m_y);
void mainMenuLeftClick(MainMenu* gui);
void mainMenuLeftClickDown(MainMenu* gui);
void mainMenuKeyUp(MainMenu* gui);
void mainMenuLeft(MainMenu* gui);
void mainMenuRight(MainMenu* gui);
void mainMenuUpdateLabel(SDL_Renderer* renderer, MainMenu* gui);
void startRun(void** args);


// Fonction de redimensionnement du menu.
void mainMenuSetScreenSize(MainMenu* gui, unsigned int w, unsigned int h);