#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "button.h"
#include "../Fight/cartes.h"
#include "../entities/card_entity.h"
#include "../utils/list.h"





// Etat de l'interface.
typedef enum {
    DK_GATCHA,
    DK_DECK
} DeckGUIState;



// Etat lors d'un pull.
typedef enum {
    PS_NONE,
    PS_PULL,
    PS_ANIM,
    PS_CHOICE
} PullState;




// Nombre de boutons dans l'interface.
#define DK_NUM_BUTTONS 5

// Structure pour l'interface des pulls et du deck.
// w,h : dimensions de la fenètre.
// m_x, m_y : position de la souris.
// state, pull_state : états de l'interface.
// buttons : liste des boutons.
// background : image de fond.
// prev_gold, gold : variables pour la quantité d'argent.
// gold_num_texture, gold_texture : textures pour l'affichage de l'argent.
// font : police d'écriture pour le rendu de texte.
// entity : entité pour l'affichage de la carte pull.
// cards : liste des cartes parmisl lesquels pull.
// card_id : id de la carte pull.
// info_alpha, dt : paramètres pour l'affichage des infobulles.
struct s_DeckGUI {
    unsigned int w;
    unsigned int h;
    unsigned int m_x;
    unsigned int m_y;
    DeckGUIState state;
    PullState pull_state;
    Button* buttons[DK_NUM_BUTTONS];
    SDL_Texture* background;
    int prev_gold;
    int* gold;
    SDL_Texture* gold_num_texture;
    SDL_Texture* gold_texture;
    TTF_Font* font;
    CardEntity* entity;
    Carte** cards;
    int* deck;
    int card_id;
    float info_alpha;
    int dt;
    List* deck_card_entities;
    float offset;
};
typedef struct s_DeckGUI DeckGUI;





// Constructeur, destructeur.
DeckGUI* createDeckGUI(SDL_Renderer* renderer, TTF_Font* font, Carte** cards, int* gold, int* deck,
    unsigned int w, unsigned int h);
void freeDeckGUI(DeckGUI* gui);



// Fonctions d'update de l'interface.
void deckGUIUpdateDeckCards(DeckGUI* gui);
void deckGUIUpdate(SDL_Renderer* renderer, DeckGUI* gui, unsigned int m_x, unsigned int m_y);
void deckGUIUpdateGoldTexture(SDL_Renderer* renderer, DeckGUI* gui);
void deckGUILeftClick(DeckGUI* gui);
void deckGUIWheelUp(DeckGUI* gui);
void deckGUIWheelDown(DeckGUI* gui);


// Fonction de redimensionnement de l'interface.
void deckGUISetScreenSize(DeckGUI* gui, unsigned int w, unsigned int h);
