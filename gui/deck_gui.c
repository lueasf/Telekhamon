#include "deck_gui.h"

#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>

#include "../graphics/font.h"
#include "../Fight/init/init_cartes.h"
#include "../graphics/transition.h"





#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))





// Fonction de génération aléatoire pondérée des cartes.
int genCard(Carte** cards) {
    
    // Parcourt les cartes pour calculer l'intervalle d'entiers correspondant à la
    // probabilité d'obtention d'une carte. Si le nombre aléatoire tombe dans cet 
    // intervalle, on choisis la carte.
    int weight[NBCARTES] = {0};
    unsigned int rarity[6] = {80,30,15,8,2,0};
    for (unsigned int i = 2; i < NBCARTES; i++) {
        if (!cards[i]) {
            weight[i] = weight[i-1];
            continue;
        }
        weight[i] = rarity[cards[i]->rare] + ((i > 0) ? weight[i-1] : 0);
    }

    // Génère le nombre aléatoire et renvoie la carte correspondante.
    int max = weight[NBCARTES-1];
    int num = rand()%(max-1)+1;
    for (unsigned int i = 2; i < NBCARTES; i++) {
        if (num <= weight[i]) return i;
    }
    return -1;

}



// Fonction de mise à jour de la position/taille de la carte d'un pull.
void updatePulledCardEntity(DeckGUI* gui) {
    
    // Redimensionne la carte.
    if (!gui->entity) return;
    gui->entity->rect.h = gui->h/3;
    gui->entity->rect.w = gui->entity->rect.h/1.5;

    // La repositionne si elle est à l'arret.
    if (!gui->entity->transition) {
        gui->entity->rect.x = (gui->w-gui->entity->rect.w)/2;
        gui->entity->rect.y = (gui->h-gui->entity->rect.h)/2;
        return;
    }

    // Redéfinit sa transition si elle est en mouvement.
    gui->entity->transition->x_i = (gui->w-gui->entity->rect.w)/2;
    gui->entity->transition->y_i = -gui->entity->rect.h;
    gui->entity->transition->x_f = (gui->w-gui->entity->rect.w)/2;
    gui->entity->transition->y_f = (gui->h-gui->entity->rect.h)/2;

}



// Fonction de mise à jour des cartes à afficher dans le deck.
void deckGUIUpdateDeckCards(DeckGUI* gui) {

    // Vide la liste des cartes.
    for (unsigned int i = 0; i < listSize(gui->deck_card_entities); i++) 
        freeCardEntity((CardEntity*)listGet(gui->deck_card_entities, i));
    listClear(gui->deck_card_entities);

    // Crée la nouvelle liste.
    for (unsigned int i = 0; i < NBCARTES; i++) {
        for (unsigned int j = 0; j < gui->deck[i]; j++) {
            listAdd(gui->deck_card_entities, createCardEntity(gui->cards[i], i));
        }
    }

    // Redimensionne les cartes.
    deckGUISetScreenSize(gui, gui->w, gui->h);

}





// Fonction du bouton pour entrer dans le deck.
void enterDeck(void** args) {
    DeckGUIState* state = args[0];
    *state = DK_DECK;
}



// Fonction du bouton pour entrer dans le gatcha.
void enterGatcha(void** args) {
    DeckGUIState* state = args[0];
    *state = DK_GATCHA;
}



// Fonction du bouton pour pull une carte.
void pullCard(void** args) {
    PullState* state = args[0];
    int* gold = args[1];
    
    // Si le joueur est assez riche, lui donne une carte.
    if (*gold >= 160 && *state == PS_NONE) {
        *state = PS_PULL;
        *gold -= 160;
    }

}



// Fonction du bouton pour garder une carte.
void keepCard(void** args) {
    DeckGUI* gui = args[0];
    if (gui->card_id >= 0) gui->deck[gui->card_id]++;
    gui->pull_state = PS_NONE;
    gui->card_id = -1;
    freeCardEntity(gui->entity);
    gui->entity = NULL;
    deckGUIUpdateDeckCards(gui);
}



// Fonction du bouton pour jeter une carte.
void discardCard(void** args) {
    PullState* pull_state = args[0];
    int* card_id = args[1];
    CardEntity** entity = args[2];
    *pull_state = PS_NONE;
    *card_id = -1;
    freeCardEntity(*entity);
    *entity = NULL;
}





// Constructeur de DeckGUI.
DeckGUI* createDeckGUI(SDL_Renderer* renderer, TTF_Font* font, Carte** cards, int* gold, int* deck,
    unsigned int w, unsigned int h) {

    // Malloc de la gui.
    DeckGUI* gui = (DeckGUI*)malloc(sizeof(DeckGUI));
    if (!gui) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du malloc de createDeckGUI");
        abort();
    }

    // Chargement de l'image de fond.
    gui->background = IMG_LoadTexture(renderer, "GameData/textures/gui/deck_background.jpeg");
    if (!gui->background) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors de la création de DeckGUI : %s", IMG_GetError());
        abort();
    }

    // Chargement de l'image de l'argent.
    gui->gold_texture = IMG_LoadTexture(renderer, "GameData/textures/gui/gold.png");
    if (!gui->gold_texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors de la création de DeckGUI : %s", IMG_GetError());
        abort();
    }

    // Initialisation des membres de l'ui.
    gui->dt = SDL_GetTicks();
    gui->info_alpha = 0.0f;
    gui->state = DK_GATCHA;
    gui->pull_state = PS_NONE;
    gui->cards = cards;
    gui->deck = deck;
    gui->card_id = -1;
    gui->entity = NULL;
    gui->m_x = -1;
    gui->m_y = -1;
    gui->offset = 0;
    gui->font = font;
    gui->gold = gold;
    gui->prev_gold = *gold+1;
    gui->gold_num_texture = NULL;
    deckGUIUpdateGoldTexture(renderer, gui);

    // Zone clicable des boutons.
    float area[8] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    
    // Bouton pour ouvrir l'interface de pull.
    void** args0 = (void**)malloc(sizeof(void*));
    if (!args0) abort();
    args0[0] = &gui->state;
    gui->buttons[0] = createButton(
        renderer, font, "GameData/textures/gui/fill_down.png", 
        "GameData/textures/gui/fill_up.png", area, "PULL", args0, enterGatcha
    );

    // Bouton pour ouvrir l'interface du deck.
    void** args1 = (void**)malloc(sizeof(void*));
    if (!args1) abort();
    args1[0] = &gui->state;
    gui->buttons[1] = createButton(
        renderer, font, "GameData/textures/gui/fill_down.png", 
        "GameData/textures/gui/fill_up.png", area, "DECK", args1, enterDeck
    );

    // Bouton pour pull.
    void** args2 = (void**)malloc(2*sizeof(void*));
    if (!args2) abort();
    args2[0] = &gui->pull_state;
    args2[1] = gui->gold;
    gui->buttons[2] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "PULL (x160)", args2, pullCard
    );

    // Bouton pour garder une carte.
    void** args3 = (void**)malloc(sizeof(void*));
    args3[0] = gui;
    gui->buttons[3] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "KEEP", args3, keepCard
    );

    // Bouton pour jeter une carte.
    void** args4 = (void**)malloc(3*sizeof(void*));
    args4[0] = &gui->pull_state;
    args4[1] = &gui->card_id;
    args4[2] = &gui->entity;
    gui->buttons[4] = createButton(
        renderer, font, "GameData/textures/gui/button_down.png", 
        "GameData/textures/gui/button_up.png", area, "DISCARD", args4, discardCard
    );

    // Crée la liste pour enregistrer les entités des cartes à afficher et 
    // initialise la taille de l'interface.
    gui->deck_card_entities = createList(16);
    deckGUISetScreenSize(gui, w,h);
    deckGUIUpdateDeckCards(gui);

    // Renvoie l'ui initialisée.
    return gui;

}



// Destructeur de DeckGUI.
void freeDeckGUI(DeckGUI* gui) {
    for (unsigned int i = 0; i < listSize(gui->deck_card_entities); i++) 
        freeCardEntity((CardEntity*)listGet(gui->deck_card_entities, i));
    freeList(gui->deck_card_entities);
    for (unsigned int i = 0; i < DK_NUM_BUTTONS; i++) freeButton(gui->buttons[i]);
    if (gui->gold_num_texture) SDL_DestroyTexture(gui->gold_num_texture);
    if (gui->gold_texture) SDL_DestroyTexture(gui->gold_texture);
    if (gui->background) SDL_DestroyTexture(gui->background);
    if (gui->entity) freeCardEntity(gui->entity);
    free(gui);
}





// Fonction de mise à jour de l'interface.
void deckGUIUpdate(SDL_Renderer* renderer, DeckGUI* gui, unsigned int m_x, unsigned int m_y) {

    // Update les coordonnées du curseur.
    if (gui->m_x != m_x || gui->m_y != m_y) gui->dt = SDL_GetTicks();
    gui->m_x = m_x;
    gui->m_y = m_y;

    // Update l'alpha des infobulles.
    if (SDL_GetTicks() - gui->dt > 500) gui->info_alpha = min(gui->info_alpha + 0.1f, 1.0f);
    else gui->info_alpha = max(gui->info_alpha - 0.1f, 0.0f);

    // Met à jour la texture de l'argent.
    deckGUIUpdateGoldTexture(renderer, gui);

    // Update de l'interface gatcha.
    if (gui->state == DK_GATCHA) {
        switch (gui->pull_state) {

        // Pull une carte en cas d'appui sur un bouton.
        case PS_PULL:
            gui->card_id = genCard(gui->cards);
            gui->entity = createCardEntity(gui->cards[gui->card_id], gui->card_id);
            gui->entity->transition = createTransition(0,0,0,0, 30, sinusTransition);
            updatePulledCardEntity(gui);
            gui->entity->rect.y = -gui->entity->rect.h;
            gui->pull_state = PS_ANIM;
            break;

        // Effectue l'animation de la carte.
        case PS_ANIM:
            if (gui->entity->transition->call(
                &gui->entity->rect.x, &gui->entity->rect.y, 
                gui->entity->transition, gui->entity->transition->data
            )) {
                freeTransition(gui->entity->transition);
                gui->entity->transition = NULL;
                gui->pull_state = PS_CHOICE;
            }
            break;
        default:
            break;
        }

    }

}



// Met à jour la texture de l'argent.
void deckGUIUpdateGoldTexture(SDL_Renderer* renderer, DeckGUI* gui) {
    if (*gui->gold == gui->prev_gold) return;
    gui->prev_gold = *gui->gold;

    // Supprime la précédente.
    if (gui->gold_num_texture) SDL_DestroyTexture(gui->gold_num_texture);

    // Crée le string à afficher.
    char text[7] = {0};
    int value = *gui->gold;
    for (unsigned int i = 0; i < 6; i++) {
        text[5-i] = '0' + value%10;
        value /= 10;
    }
    
    // Génère la texture.
    SDL_Color color = {0x00,0x00,0x00,0xFF};
    gui->gold_num_texture = createTextureFromText(renderer, gui->font, &color, text);

}



// Fonction de gestion du click 
void deckGUILeftClick(DeckGUI* gui) {

    // Boutons pour changer le menu.
    onClick(gui->buttons[0], gui->m_x, gui->m_y);
    onClick(gui->buttons[1], gui->m_x, gui->m_y);

    // Gestions des clicks dans le gatcha.
    if (gui->state == DK_GATCHA) {
        switch (gui->pull_state) {

        // Bouton de pull.
        case PS_NONE:
            onClick(gui->buttons[2], gui->m_x, gui->m_y);
            break;

        // Boutons pour concerver ou non les cartes.
        case PS_CHOICE:
            onClick(gui->buttons[3], gui->m_x, gui->m_y);
            onClick(gui->buttons[4], gui->m_x, gui->m_y);
            break;
        default:
            break;
        }
    } 

}



// Déplace les cartes avec un roulement de molette vers le haut.
void deckGUIWheelUp(DeckGUI* gui) {
    gui->offset -= 0.25f;
    if (gui->offset < 0.0f) gui->offset = 0.0f;
}



// Déplace les cartes avec un roulement de molette vers le bas.
void deckGUIWheelDown(DeckGUI* gui) {
    gui->offset += 0.25f;
    float screen_height = gui->h/(1.5*gui->w/5);
    float max_val = max(0,((int)listSize(gui->deck_card_entities)-1)/4+1.5-screen_height);
    if (gui->offset > max_val) gui->offset = max_val;
}





// Fonction de redimensionnement de l'ui.
void deckGUISetScreenSize(DeckGUI* gui, unsigned int w, unsigned int h) {
    gui->w = w;
    gui->h = h;

    // Redimensionne les boutons de la barre du haut.
    gui->buttons[0]->rect.w = w/2+1;
    gui->buttons[0]->rect.h = 0.05*h;
    gui->buttons[0]->rect.x = 0;
    gui->buttons[0]->rect.y = 0;
    gui->buttons[1]->rect.w = w/2+1;
    gui->buttons[1]->rect.h = 0.05*h;
    gui->buttons[1]->rect.x = w/2+1;
    gui->buttons[1]->rect.y = 0;

    // Redimensionne le bouton de pull.
    gui->buttons[2]->rect.w = 0.25*gui->w;
    gui->buttons[2]->rect.h = 0.2*gui->buttons[2]->rect.w;
    gui->buttons[2]->rect.x = (gui->w-gui->buttons[2]->rect.w)/2;
    gui->buttons[2]->rect.y = (gui->h-gui->buttons[2]->rect.h)/2;

    // Redimensionne les boutons de choix de carte.
    for (unsigned int i = 0; i < 2; i++) {
        gui->buttons[3+i]->rect.w = 0.25*gui->w;
        gui->buttons[3+i]->rect.h = 0.2*gui->buttons[3+i]->rect.w;
        gui->buttons[3+i]->rect.x = (gui->w/2-1.05*(1-i)*gui->buttons[3+i]->rect.w) + 0.05*gui->buttons[3+i]->rect.w;
        gui->buttons[3+i]->rect.y = gui->h-0.3*gui->h;
    }

    // Repositionne la carte.
    updatePulledCardEntity(gui);

    // Repositionne les cartes du deck.
    for (unsigned int i = 0; i < listSize(gui->deck_card_entities); i++) {
        CardEntity* entity = listGet(gui->deck_card_entities, i);
        entity->rect.w = gui->w/5;
        entity->rect.h = 1.5*gui->w/5;
        entity->rect.x = (1+6*(i%4))*entity->rect.w/5;
        entity->rect.y = (1+6*(i/4))*entity->rect.h/5;
    }

}
