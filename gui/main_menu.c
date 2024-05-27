#include "main_menu.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <string.h>

#include "../graphics/font.h"
#include "deck_gui.h"




// Bornes des levels.
#define MIN_LEVEL 2
#define MAX_LEVEL 5
#define MIN_DIFF 0
#define MAX_DIFF 2





// Fonction pour quitter le jeu.
void exitGame(void** args) {
    int* run = args[0];
    *run = 0;
}

// Fonction pour accéder aux options.
void enterMainOptions(void** args) {
    MainMenuState* state = args[0];
    *state = MMS_OPTIONS;
}

// Fonction entrer dans le menu de création de run.
void enterNewRun(void** args) {
    MainMenuState* state = args[0];
    *state = MMS_NEW_RUN;
}

// Fonction pour retourner au menu principal.
void returnMainMenu(void** args) {
    MainMenuState* state = args[0];
    *state = MMS_MAIN;
}

// Fonction pour accéder aux crédits.
void enterCredits(void** args) {
    MainMenuState* state = args[0];
    *state = MMS_CREDITS;
}

// Fonction pour augmenter le nombre de niveaux.
void increaseLevelNumber(void** args) {
    MainMenu* menu = (MainMenu*)args[0];
    if (menu->level_number < MAX_LEVEL) menu->level_number++;
    mainMenuUpdateLabel(args[1], menu);
}

// Fonction pour diminuer le nombre de niveaux.
void decreaseLevelNumber(void** args) {
    MainMenu* menu = (MainMenu*)args[0];
    if (menu->level_number > MIN_LEVEL) menu->level_number--;
    mainMenuUpdateLabel(args[1], menu);
}

// Fonction pour augmenter le difficulté.
void increaseDifficulty(void** args) {
    MainMenu* menu = (MainMenu*)args[0];
    if (menu->difficulty < MAX_DIFF) menu->difficulty++;
    mainMenuUpdateLabel(args[1], menu);
}

// Fonction pour diminuer le difficulté.
void decreaseDifficulty(void** args) {
    MainMenu* menu = (MainMenu*)args[0];
    if (menu->difficulty > MIN_DIFF) menu->difficulty--;
    mainMenuUpdateLabel(args[1], menu);
}


void startRun(void** args) {
    Map** map_p = (Map**)args[0];
    Player** player_p = (Player**)args[1];
    LevelEntity** entity_p = (LevelEntity**)args[2];
    int* difficulty = (int*)args[3];
    DeckGUI* gui = (DeckGUI*)args[4];
    TileSet** tilesets = (TileSet**)args[5];
    TileSet* ennemi_map_tileset = (TileSet*)args[6];
    TileSet* momie_tileset = (TileSet*)args[7];
    int* width = (int*)args[8];
    int* height = (int*)args[9];
    GameState* game_state = (GameState*)args[10];
    SDL_Renderer* renderer = (SDL_Renderer*)args[11];

    for (int i = 0; i < 26; i++) {
        (*player_p)->deck[i] = 0;
    }
    (*player_p)->deck[0] = 5;
    (*player_p)->deck[1] = 5;
    (*player_p)->MHP = 150;
    (*player_p)->HP = (*player_p)->MHP;
    (*player_p)->frame = 0;
    for (int i = 0; i < 7; i++) {
        (*player_p)->status[i] = 0;
    }
    (*player_p)->gold = 1600 - 800*(*difficulty);
    gui->gold = &(*player_p)->gold;
    deckGUIUpdateGoldTexture(renderer, gui);
    deckGUIUpdateDeckCards(gui);
    
    if (*map_p) freeMap(*map_p);
    *map_p = generateMap(0, 100, 100, tilesets, *entity_p, ennemi_map_tileset);

    *game_state = OVERWORLD;
    
}


// Constructeur de MainMenu. 
MainMenu* createMainMenu(SDL_Renderer* renderer, TTF_Font* font, unsigned int w, unsigned int h, 
    int* run, void** argsStartRun) {

    // Malloc le menu.
    MainMenu* menu = (MainMenu*)malloc(sizeof(MainMenu));
    if (!menu) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du malloc de createMainMenu");
        abort();
    }

    // Carge l'image du background.
    menu->background = IMG_LoadTexture(renderer, "GameData/textures/gui/main_menu_background.jpeg");
    if (!menu->background) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors de la création du TileSet : %s", IMG_GetError());
        abort();
    }

    //Met à zéro les composants.
    menu->font = font;
    menu->m_x = 0;
    menu->m_y = 0;
    menu->state = MMS_MAIN;
    menu->level_number = 3;
    menu->difficulty = 1;
    menu->level_number_label = NULL;
    menu->difficulty_label = NULL;
    mainMenuUpdateLabel(renderer, menu);

    // Initialise le slider pour le volume.
    menu->volume = createSlider(0.5f);
    Mix_Volume(-1, 0.12f*MIX_MAX_VOLUME);
    Mix_VolumeMusic(0.5f*MIX_MAX_VOLUME);

    // Initialise la textbox.
    menu->box = createTextBox(font, 20, 1);

    // Crée le label pour le titre du jeu.
    SDL_Color color = {0xF0, 0xF0, 0xF0, 0xFF};
    SDL_Color out_color = {0x00, 0x00, 0x00, 0x80};
    menu->main_label = createTextureFromTextOutline(renderer, font, &color, &out_color, 4, "TELEKHAMON");
    out_color.a = 0xFF;
    menu->volume_label = createTextureFromText(renderer, font, &out_color, "VOLUME");

    // Crée la texture des crédits.
    menu->credits = createTextureFromTextFromated(
        renderer, font,
        "\effffffCe jeu a ete realisé avec \eff0000 PASSION \effffff bien sûr par : \n \n"
        "CORREIA Lucie  \n"
        "FOULOU Louis  \n"
        "PADELLINI Alexandre  \n"
        "SENTENAC Eliott  \n"
        "THUR Adrien \n \n \n \n \n \n \n \n"
        "\e080804 L'Empereur-Dieu de Dune c'est le meilleur bouquin du cycle"
    );

    // Area pour les boutons.
    float area[8] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    
    // Bouton pour ouvrir l'interface de création de run.
    void** args0 = (void**)malloc(sizeof(void*));
    if (!args0) abort();
    args0[0] = &menu->state;
    menu->buttons[0] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "NEW GAME", args0, enterNewRun
    );
    
    // Bouton pour les options.
    void** args1 = (void**)malloc(sizeof(void*));
    if (!args1) abort();
    args1[0] = &menu->state;
    menu->buttons[1] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "OPTIONS", args1, enterMainOptions
    );

    // Bouton pour les crédits.
    void** args2 = (void**)malloc(sizeof(void*));
    if (!args2) abort();
    args2[0] = &menu->state;
    menu->buttons[2] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "CREDITS", args2, enterCredits
    );

    // Bouton pour quitter le jeu.
    void** args3 = (void**)malloc(sizeof(void*));
    if (!args3) abort();
    args3[0] = run;
    menu->buttons[3] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "QUIT GAME", args3, exitGame
    );

    // Bouton pour revenir à la partie principale du menu.
    void** args4 = (void**)malloc(sizeof(void*));
    if (!args4) abort();
    args4[0] = &menu->state;
    menu->buttons[4] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "RETURN", args4, returnMainMenu
    );

    // Bouton pour diminuer le nombre de niveaux.
    void** args5 = (void**)malloc(2*sizeof(void*));
    if (!args5) abort();
    args5[0] = menu;
    args5[1] = renderer;
    menu->buttons[5] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "<", args5, decreaseLevelNumber
    );

    // Bouton pour augmenter le nombre de niveaux.
    void** args6 = (void**)malloc(2*sizeof(void*));
    if (!args6) abort();
    args6[0] = menu;
    args6[1] = renderer;
    menu->buttons[6] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, ">", args6, increaseLevelNumber
    );

    // Bouton pour diminuer la difficulté.
    void** args7 = (void**)malloc(2*sizeof(void*));
    if (!args7) abort();
    args7[0] = menu;
    args7[1] = renderer;
    menu->buttons[7] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "<", args7, decreaseDifficulty
    );

    // Bouton pour augmenter la difficulté.
    void** args8 = (void**)malloc(2*sizeof(void*));
    if (!args8) abort();
    args8[0] = menu;
    args8[1] = renderer;
    menu->buttons[8] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, ">", args8, increaseDifficulty
    );

    // Bouton pour lancer une run.
    argsStartRun[3] = &menu->difficulty;
    menu->buttons[9] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "START", argsStartRun, startRun
    );

    // Initialise la taille du menu.
    mainMenuSetScreenSize(menu, w,h);

    // Renvoie le menu initialisé.
    return menu;

}



// Destructeur de MainMenu.
void freeMainMenu(MainMenu* gui) {
    freeTextBox(gui->box);
    freeSlider(gui->volume);
    for (unsigned int i = 0; i < MM_NUM_BUTTONS; i++) freeButton(gui->buttons[i]);
    if (gui->difficulty_label) SDL_DestroyTexture(gui->difficulty_label);
    if (gui->level_number_label) SDL_DestroyTexture(gui->level_number_label);
    if (gui->volume_label) SDL_DestroyTexture(gui->volume_label);
    if (gui->main_label) SDL_DestroyTexture(gui->main_label);
    if (gui->background) SDL_DestroyTexture(gui->background);
    if (gui->credits) SDL_DestroyTexture(gui->credits);
    free(gui);
}





// Fonction de gestion des events clavier.
void mainMenuProcessEvent(MainMenu* gui, SDL_Renderer* renderer, SDL_Keycode code) {
    if (gui->state == MMS_NEW_RUN) textBoxProcessEvent(gui->box, renderer, code);
}



// Fonction d'update du menu.
void mainMenuUpdate(SDL_Renderer* renderer, MainMenu* gui, unsigned int m_x, unsigned int m_y) {
    gui->m_x = m_x;
    gui->m_y = m_y;
    if (gui->state == MMS_OPTIONS) {
        if (sliderUpdate(gui->volume, m_x)) {
            Mix_Volume(-1, 0.25*gui->volume->value*MIX_MAX_VOLUME);
            Mix_VolumeMusic(gui->volume->value*MIX_MAX_VOLUME);
        }
    }
}



// Fonction de gestion de clic gauche.
void mainMenuLeftClick(MainMenu* gui) {

    // Execute le clic gauche sur les bon boutons.
    switch (gui->state) {
    case MMS_MAIN:
        for (unsigned int i = 0; i < 4; i++) onClick(gui->buttons[i], gui->m_x, gui->m_y);
        break;
    case MMS_CREDITS:
        mainMenuKeyUp(gui);
        break;
    case MMS_OPTIONS:
        onClick(gui->buttons[4], gui->m_x, gui->m_y);
        sliderLeftClickReleased(gui->volume);
        break;
    case MMS_NEW_RUN:
        for (unsigned int i = 4; i < 9; i++) onClick(gui->buttons[i], gui->m_x, gui->m_y);
        if (gui->box->text_size > 0) onClick(gui->buttons[9], gui->m_x, gui->m_y);
        textBoxLeftClick(gui->box, gui->m_x, gui->m_y);
        break;
    }

}



// Fonction de gestion du clic gauche.
void mainMenuLeftClickDown(MainMenu* gui) {
    if (gui->state == MMS_OPTIONS) sliderLeftClick(gui->volume, gui->m_x, gui->m_y);
}



// Gestion des eventments de touche relâchée.
void mainMenuKeyUp(MainMenu* gui) {
    if (gui->state == MMS_CREDITS) {
        gui->state = MMS_MAIN;
    } 
}



// Gestion de le flèche gauche.
void mainMenuLeft(MainMenu* gui) {
    if (gui->state == MMS_NEW_RUN) textBoxLeft(gui->box);
}



// Gestion de le flèche droite.
void mainMenuRight(MainMenu* gui) {
    if (gui->state == MMS_NEW_RUN) textBoxRight(gui->box);
}





// Met à jour les labels du menu.
void mainMenuUpdateLabel(SDL_Renderer* renderer, MainMenu* gui) {
    if (gui->difficulty_label) SDL_DestroyTexture(gui->difficulty_label);
    if (gui->level_number_label) SDL_DestroyTexture(gui->level_number_label);

    // Crée les nouveaux labels.
    SDL_Color black = {0x00, 0x00, 0x00, 0xFF};
    char num[3] = {'0'+gui->level_number/10, '0'+gui->level_number%10, 0};
    char* diff[3] = {"easy", "normal", "hard"};
    gui->level_number_label = createTextureFromText(renderer, gui->font, &black, num);
    gui->difficulty_label = createTextureFromText(renderer, gui->font, &black, diff[gui->difficulty]);

}





// Fonction de redimensionnement.
void mainMenuSetScreenSize(MainMenu* gui, unsigned int w, unsigned int h) {
    gui->w = w;
    gui->h = h;

    // Récupère la taille de l'arrière plan.
    int img_w, img_h;
    SDL_QueryTexture(gui->background, NULL, NULL, &img_w, &img_h);
    
    // La redimensionne en fonction de la taille de la fenètre.
    float img_ratio = img_w/(float)img_h;
    float ratio = w/(float)h;
    if (img_ratio > ratio) {
        gui->background_rect.w = h*img_ratio;
        gui->background_rect.h = h;
        gui->background_rect.x = (w-h*img_ratio)/2;
        gui->background_rect.y = 0;
    } else {
        gui->background_rect.w = w;
        gui->background_rect.h = w/img_ratio;
        gui->background_rect.x = 0;
        gui->background_rect.y = (h-w/img_ratio)/2;
    }

    // Calcule la taille d'un bouton.
    gui->button_rect.w = w/4;
    gui->button_rect.h = gui->button_rect.w/5;
    if (gui->button_rect.h*11 > h) {
        float ratio = h/(float)(gui->button_rect.h*11.0f);
        gui->button_rect.w = gui->button_rect.w*ratio;
        gui->button_rect.h = gui->button_rect.h*ratio;
    }

    // Redimensionne et positionne les boutons.
    for (unsigned int i = 0; i < 5; i++) {
        gui->buttons[i]->rect.x = (w-gui->button_rect.w)/2;
        gui->buttons[i]->rect.y = (1.5*i+4)*gui->button_rect.h;
        gui->buttons[i]->rect.w = gui->button_rect.w;
        gui->buttons[i]->rect.h = gui->button_rect.h;
    }
    for (unsigned int i = 0; i < 4; i++) {
        gui->buttons[5+i]->rect.x = (w-gui->button_rect.w-gui->button_rect.h)/2 + (i%2)*gui->button_rect.w;
        gui->buttons[5+i]->rect.y = (5.5 + 1.5*(i/2))*gui->button_rect.h;
        gui->buttons[5+i]->rect.w = gui->button_rect.h;
        gui->buttons[5+i]->rect.h = gui->button_rect.h;
    }
    gui->buttons[9]->rect.x = (w-gui->button_rect.w)/2;
    gui->buttons[9]->rect.y = (8.5)*gui->button_rect.h;
    gui->buttons[9]->rect.w = gui->button_rect.w;
    gui->buttons[9]->rect.h = gui->button_rect.h;

    // Redimensionne et positionne la textbox.
    SDL_Rect text_box_rect = {
        (w-gui->button_rect.w)/2.0f, 4.0f*gui->button_rect.h,
        gui->button_rect.w, gui->button_rect.h
    };
    textBoxResize(gui->box, &text_box_rect);

    // Redimensionne et positionne le slider du volume.
    SDL_Rect slider_rect = {
        (w-gui->button_rect.w)/2.0f, 5.0f*gui->button_rect.h,
        gui->button_rect.w, gui->button_rect.h
    };
    sliderResize(gui->volume, &slider_rect);

}
