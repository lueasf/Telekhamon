#include "card_entity.h"

#include <SDL2/SDL.h>
#include <stdlib.h>




// Constructeur de CardEntity.
CardEntity* createCardEntity(Carte* carte, int id) {
    CardEntity* entity = (CardEntity*)malloc(sizeof(CardEntity));
    if (!entity) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour CardEntity");
        abort();
    }

    // Initialise l'entity et la renvoie.
    entity->transition = NULL;
    entity->carte = carte;
    entity->alpha = 1.0f;
    entity->rect.x = 0;
    entity->rect.y = 0;
    entity->rect.w = 0;
    entity->rect.h = 0;
    entity->id = id;

    // Renvoie l'entity initialisée.
    return entity;

}



// Destructeur de CardEntity.
void freeCardEntity(CardEntity* entity) {
    if (!entity) return;
    if (entity->transition != NULL) freeTransition(entity->transition);
    free(entity);
}





// Permet de définir la position d'une carte à l'écran.
void cardSetPosition(CardEntity* entity, unsigned int x, unsigned int y) {
    entity->rect.x = x;
    entity->rect.y = y;
}



// Permet de déplacer la carte à l'écran.
void cardMove(CardEntity* entity, unsigned int dx, unsigned int dy) {
    entity->rect.x += dx;
    entity->rect.y += dy;
}





// Vérifie si l'entitée est au dessus du point passé en paramètre.
int cardIsOver(CardEntity* card, unsigned int x, unsigned int y) {
    return x >= card->rect.x && y > card->rect.y && 
        x <= card->rect.x+card->rect.w && x <= card->rect.y+card->rect.h;  
}





// Ajoute la transition à la carte et supprime la précédente s'il y en a une.
void cardAddTransition(CardEntity* entity, Transition* transition) {
    if (!entity->transition) freeTransition(entity->transition);
    entity->transition = transition;
}





// Fonction de transition de carte avec fondue en alpha.
int cardTransitionAlpha(int* x, int* y, Transition* transition, void* data) {

    // Translation linéaire.
    *x = transition->x_i + transition->k/(float)transition->n*(transition->x_f - transition->x_i);
    *y = transition->y_i + transition->k/(float)transition->n*(transition->y_f - transition->y_i);
    
    // Fondue en alpha linéaire.
    CardEntity* entity = (CardEntity*)data;
    entity->alpha = 1.0f - transition->k/(float)transition->n;

    // Passe à la prochaine étape.
    transition->k++;
    return transition->k == transition->n+1;

}
