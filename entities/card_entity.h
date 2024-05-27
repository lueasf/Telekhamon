#pragma once

#include <SDL2/SDL.h>

#include "../Fight/cartes.h"
#include "../graphics/transition.h"
#include "../Fight/cartes.h"





// Structure pour l'affichage d'une carte à l'écran.
struct s_CardEntity {
    Transition* transition;
    SDL_Rect rect;
    Carte* carte;
    float alpha;
    int id;
};
typedef struct s_CardEntity CardEntity;





// Constructeur et destructeur.
CardEntity* createCardEntity(Carte* carte, int id);
void freeCardEntity(CardEntity* entity);



// Fonctions de manipulation.
void cardSetPosition(CardEntity* entity, unsigned int x, unsigned int y);
void cardMove(CardEntity* entity, unsigned int dx, unsigned int dy);
int cardIsOver(CardEntity* card, unsigned int x, unsigned int y);



// Fonction d'ajout de transition.
void cardAddTransition(CardEntity* entity, Transition* transition);



// Fonction de transition de carte avec fondue en alpha.
int cardTransitionAlpha(int* x, int* y, Transition* transition, void* data);
