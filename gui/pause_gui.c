#include "pause_gui.h"

#include <stdlib.h>
#include <SDL2/SDL_mixer.h>

#include "../graphics/font.h"





// Fonctions pour quitter la pause.
void exitPause(void** args) {
    int* pause = args[0];
    *pause = 0;
}

// Fonctions pour ouvrir les options.
void enterOptions(void** args) {
    PauseGUIState* state = args[0];
    *state = PGS_OPTIONS;
}

// Fonctions pour quitter les options.
void exitOptions(void** args) {
    PauseGUIState* state = args[0];
    *state = PGS_PAUSE;
}

// Fonctions pour retourner au menu principal.
void exitToMainMenu(void** args) {
    int* pause = args[0];
    GameState* state = args[1];
    int* mm_state = args[2];
    *pause = 0; 
    *state = 0; 
    *mm_state = 0;
}





// Constructeur pour PauseGUI. 
PauseGUI* createPauseGUI(SDL_Renderer* renderer, TTF_Font* font, unsigned int w, unsigned int h,
    int* pause, GameState* game_state, int* mm_state) {

    // Malloc la mémoire pour le menu.
    PauseGUI* gui = (PauseGUI*)malloc(sizeof(PauseGUI));
    if (!gui) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour PauseGUI");
        abort();
    }

    // Crée le label du menu.
    SDL_Color color = {0xF0, 0xF0, 0xF0, 0xFF};
    SDL_Color out_color = {0x00, 0x00, 0x00, 0x80};
    gui->pause_label = createTextureFromTextOutline(renderer, font, &color, &out_color, 4, "PAUSE");
    gui->volume_label = createTextureFromText(renderer, font, &color, "VOLUME");

    // Etat du menu.
    gui->state = PGS_PAUSE;

    // Initialise le slider pour le volume.
    gui->volume = createSlider(0.5f);

    // Zone de clic des boutons.
    float area[8] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    
    // Boutons du menu pause.
    void** args0 = (void**)malloc(sizeof(void*));
    if (!args0) abort();
    *args0 = pause;
    gui->buttons[0] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "RESUME", args0, exitPause
    );

    // Bouton des options.
    void** args1 = (void**)malloc(sizeof(void*));
    if (!args1) abort();
    *args1 = &gui->state;
    gui->buttons[1] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "OPTIONS", args1, enterOptions
    );

    // Bouton de retour au menu principal.
    void** args2 = (void**)malloc(3*sizeof(void*));
    if (!args2) abort();
    args2[0] = &gui->state;
    args2[1] = game_state;
    args2[2] = mm_state;
    gui->buttons[2] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "RETURN MAIN MENU", args2, exitToMainMenu
    );

    // Bouton de retour des options. 
    void** args3 = (void**)malloc(sizeof(void*));
    if (!args3) abort();
    *args3 = &gui->state;
    gui->buttons[3] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "RETURN", args3, exitOptions
    );

    // Initialise la taille des boutons.
    gui->button_rect.x = 0;
    gui->button_rect.y = 0;
    pauseGUISetScreenSize(gui, w,h);
    
    // Renvoie la GUI initialisée.
    return gui;

}



// Destructeur pour PauseGUI.
void freePauseGUI(PauseGUI* pause_gui) {
    if (pause_gui->pause_label) SDL_DestroyTexture(pause_gui->pause_label);
    if (pause_gui->volume_label)SDL_DestroyTexture(pause_gui->volume_label);
    freeSlider(pause_gui->volume);
    for (unsigned int i = 0; i < 4; i++) freeButton(pause_gui->buttons[i]);
    free(pause_gui);
}





// Fonction d'update.
void pauseGUIUpdate(PauseGUI* gui, unsigned int m_x, unsigned int m_y) {
    gui->m_x = m_x;
    gui->m_y = m_y;
    
    // Update du slider de volume.
    if (gui->state == PGS_OPTIONS) {
        if (sliderUpdate(gui->volume, gui->m_x)) {
            Mix_Volume(-1, 0.25*gui->volume->value*MIX_MAX_VOLUME);
            Mix_VolumeMusic(gui->volume->value*MIX_MAX_VOLUME);
        }
        gui->main_volume->value = gui->main_volume->value;
    }

}



// Fonction de gestion du clic gauche.
void pauseGUILeftClick(PauseGUI* gui) {
    switch (gui->state) {
    case PGS_PAUSE:
        for (unsigned int i = 0; i < 3; i++) onClick(gui->buttons[i], gui->m_x, gui->m_y);
        break;
    case PGS_OPTIONS:
        sliderLeftClickReleased(gui->volume);
        onClick(gui->buttons[3], gui->m_x, gui->m_y);
        break;
    }
}



// Fonction de gestion du clic gauche.
void pauseGUILeftClickDown(PauseGUI* gui) {
    if (gui->state == PGS_OPTIONS) sliderLeftClick(gui->volume, gui->m_x, gui->m_y);
}





// Fonction de redimensionnement.
void pauseGUISetScreenSize(PauseGUI* gui, unsigned int w, unsigned int h) {
    gui->w = w;
    gui->h = h;

    // Met à jour la taille des boutons.
    gui->button_rect.w = w/4;
    gui->button_rect.h = gui->button_rect.w/5;

    // Redimensionne et positionne les boutons.
    if (gui->button_rect.h*11 > h) {
        float ratio = h/(float)(gui->button_rect.h*11.0f);
        gui->button_rect.w = gui->button_rect.w*ratio;
        gui->button_rect.h = gui->button_rect.h*ratio;
    }
    for (unsigned int i = 0; i < 3; i++) {
        gui->buttons[i]->rect.x = (w-gui->button_rect.w)/2;
        gui->buttons[i]->rect.y = (2*i+5)*gui->button_rect.h;
        gui->buttons[i]->rect.w = gui->button_rect.w;
        gui->buttons[i]->rect.h = gui->button_rect.h;
    }
    gui->buttons[3]->rect.x = (w-gui->button_rect.w)/2;
    gui->buttons[3]->rect.y = 9*gui->button_rect.h;
    gui->buttons[3]->rect.w = gui->button_rect.w;
    gui->buttons[3]->rect.h = gui->button_rect.h;
    
    // Redimensionne et positionne le slider du volume.
    SDL_Rect slider_rect = {
        (w-gui->button_rect.w)/2.0f, 5.0f*gui->button_rect.h,
        gui->button_rect.w, gui->button_rect.h
    };
    sliderResize(gui->volume, &slider_rect);

}
