#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "../entities/card_entity.h"
#include "../utils/list.h"
#include "../Fight/player.h"
#include "../Fight/init/init_cartes.h"
#include "../Fight/ennemi.h"
#include "button.h"





// Structure pour modéliser une main.
// w,h : dimension de l'écran.
// m_x, m_y, dt : position et temps d'immobilité de la souris.
// over : carte survolée par le curseur.
// x_ratio : ratio main/largeur écran.
// card_ratio : ratio hauteur/largeur des cartes.
// card_rect : rect des cartes pour l'affichage.
// cards : liste des cartes en main. Liste de CardEntity*.
// used : carte qui a été cliquée.
// used_id : id de cette carte avant sa supression de la main.
struct s_Hand {
    unsigned int w;
    unsigned int h;
    unsigned int m_x;
    unsigned int m_y;
    unsigned long dt;
    float info_alpha;
    int over;
    float x_ratio;
    float card_ratio;
    SDL_Rect card_rect;
    List* cards;
    CardEntity* used;
    int used_id;
    int* used_cards;
};
typedef struct s_Hand Hand; 





// Constructeur et destructeur.
Hand* createHand(SDL_Renderer* renderer, int* used, float x_ratio, float card_ratio, unsigned int w, unsigned int h);
void freeHand(Hand* hand);



// Fonctions d'update de la main.
int handUpdate(Hand* hand, Player* player, List* enemies, unsigned int m_x, unsigned int m_y);
void handLeftClick(Hand* hand, Player* player, Ennemi* target, List* enemies, Mix_Chunk* drawCardWav);
void handAddTransitions(Hand* hand, int speed);
void handSetScreenSize(Hand* hand, unsigned int w, unsigned int h);



// Fonctions de manipulation de cartes.
void handAddCard(Hand* hand, CardEntity* entity);
void getCardPosition(Hand* hand, int* x, int* y, unsigned int id);
