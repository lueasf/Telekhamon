#include "card_hand.h"

#include <stdlib.h>
#include <SDL2/SDL.h>

#include "../Fight/cartes.h"
#include "../Fight/ennemi.h"
#include "../Fight/casts_card.h"



#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))




// Constructeur de Hand.
// w,h : dimension de l'écran.
// x_ratio : ratio main/largeur écran.
// card_ratio : ratio hauteur/largeur des cartes.
Hand* createHand(SDL_Renderer* renderer, int* used, float x_ratio, float card_ratio, unsigned int w, unsigned int h) {
    Hand* hand = (Hand*)malloc(sizeof(Hand));
    if (!hand) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Hand");
        abort();
    }

    // Initialise la main.
    hand->x_ratio = x_ratio;
    hand->card_ratio = card_ratio;
    hand->cards = createList(10);
    hand->used_cards = used;
    hand->used = NULL;
    hand->used_id = -1;
    hand->m_x = 0;
    hand->m_y = 0;
    hand->dt = SDL_GetTicks();
    hand->info_alpha = 0;

    // Calcule le rect des cartes.
    handSetScreenSize(hand, w,h);
    hand->card_rect.x = 0;
    hand->card_rect.y = 0;
    return hand;

}



// Destructeur de Hand.
void freeHand(Hand* hand) {
    if (!hand) return;
    for (unsigned int i = 0; i < listSize(hand->cards); i++) freeCardEntity(listGet(hand->cards, i));
    if (hand->used) freeCardEntity(hand->used);
    freeList(hand->cards);
    free(hand);
}





// Met à jour la main.
// m_x, m_y : coordonnées de la souris.
int handUpdate(Hand* hand, Player* player, List* enemies, unsigned int m_x, unsigned int m_y) {
    
    // Parcourt les cartes.
    int over = -1;
    for (unsigned int i = 0; i < listSize(hand->cards); i++) {
        CardEntity* entity = (CardEntity*)listGet(hand->cards, i);

        // Détecte la carte survolée par le curseur.
        if (entity->rect.x <= m_x && entity->rect.y <= m_y &&
            entity->rect.x+entity->rect.w >= m_x && entity->rect.y+entity->rect.h >= m_y) {
            over = i;
        }

        // Effectue les transitions des cartes.
        if (entity->transition && entity->transition->call(
            &entity->rect.x, &entity->rect.y, entity->transition, NULL
        )) {
            freeTransition(entity->transition);
            entity->transition = NULL;
        }

    }

    // Repositionne les cartes si celle qui est survolée à changée.
    if (hand->over != over) {
        hand->over = over;
        handAddTransitions(hand, 10);
    }

    // Si une carte a été cliquée, effectue sa transition puis la supprime.
    int card_used = 0;
    if (hand->used) {
        if (hand->used->transition && hand->used->transition->call(
            &hand->used->rect.x, &hand->used->rect.y, hand->used->transition, hand->used
        )) {

            // Reset les animations et supprime les ennemis morts.
            player->frame = (SDL_GetTicks()/250)%4;
            for (int i = listSize(enemies)-1; i >= 0; i--) {
                Ennemi* enemy = (Ennemi*)listGet(enemies, i);
                enemy->frame = (SDL_GetTicks()/250)%4;
                if (enemy->HP == 0) FreeEnnemi(enemy, enemies);
            } 

            // Supprime la carte et la met dans la pile de défausse.
            if (!hand->used->carte->exhaust) hand->used_cards[hand->used->id]++;
            freeCardEntity(hand->used);
            hand->used = NULL;
            hand->used_id = -1;
            card_used = 1;

        }
    }

    // Update du temps d'inactivité de la souris.
    if (hand->m_x != m_x || hand->m_y != m_y) hand->dt = SDL_GetTicks();
    hand->m_x = m_x;
    hand->m_y = m_y;

    // Gestion de l'alpha de l'infobulle.
    if (SDL_GetTicks() - hand->dt > 500) hand->info_alpha = min(hand->info_alpha + 0.1f, 1.0f);
    else hand->info_alpha = max(hand->info_alpha - 0.1f, 0.0f);

    return card_used;

}



// Gestion du left click.
void handLeftClick(Hand* hand, Player* player, Ennemi* target, List* enemies, Mix_Chunk* drawCardWav) {
    if (hand->over != -1 && hand->used == NULL) {
        CardEntity* card = (CardEntity*)listGet(hand->cards, hand->over);
        if (card->carte->cout > player->energy) return;

        // Joue le son du piochage
        Mix_PlayChannel(-1, drawCardWav, 0);

        // Utilise la carte sélectionnée.
        player->energy -= card->carte->cout;
        hand->used = card;
        hand->used_id = hand->over;
        hand->dt = SDL_GetTicks();

        // Récupère les pvs de tout les ennemis.
        for (unsigned int i = 0; i < listSize(enemies); i++) {
            Ennemi* enemy = (Ennemi*)listGet(enemies, i);
            enemy->last_hp = enemy->HP;
        } 

        // Applique les effets d'une carte.
        ApplyCard(player, card->carte, 0, target, enemies);

        // Met à jour les animations de dégat.
        for (unsigned int i = 0; i < listSize(enemies); i++) {
            Ennemi* enemy = (Ennemi*)listGet(enemies, i);
            if (enemy->last_hp != enemy->HP) enemy->frame = 5;
        } 

        // Met à jour l'animation d'attaque.
        player->frame = 4;            

        // Ajoute une fondue en alpha avec une translation vers le haut si la carte est cliquée.
        cardAddTransition(hand->used, createTransition(
            hand->used->rect.x, hand->used->rect.y, hand->used->rect.x, 
            hand->used->rect.y - hand->used->rect.h/2, 10, cardTransitionAlpha
        ));

        // Supprime la carte de la liste et repositionne les autres.
        listRemove(hand->cards, hand->over);
        handAddTransitions(hand, 10);
    }
}



// Ajoute des transitions aux cartes de la main pour les replacer en speed frames.
void handAddTransitions(Hand* hand, int speed) {
    int x_f, y_f;
    for (unsigned int i = 0; i < listSize(hand->cards); i++) {
        getCardPosition(hand, &x_f, &y_f, i);
        CardEntity* card = listGet(hand->cards, i);
        cardAddTransition(card, createTransition(
            card->rect.x, card->rect.y, x_f, y_f, speed, linearTransition
        ));
    }
}



// Fonction de mise à jour des dimension en cas de redimensionnement de l'écran.
void handSetScreenSize(Hand* hand, unsigned int w, unsigned int h) {
    
    // Recalcule la taille des cartes.
    hand->card_rect.w = w*hand->x_ratio/5.5;
    hand->card_rect.h = hand->card_rect.w*hand->card_ratio;
    hand->w = w;
    hand->h = h;
    hand->over = -1;

    // Parcours des cartes.
    for (unsigned int i = 0; i < listSize(hand->cards); i++) {
        CardEntity* entity = (CardEntity*)listGet(hand->cards, i);
        entity->rect.w = hand->card_rect.w;
        entity->rect.h = hand->card_rect.h;

        // Si une transition existe, change son point d'arrivée.
        int x, y;
        getCardPosition(hand, &x, &y, i);
        if (entity->transition) {
            entity->transition->x_f = x;
            entity->transition->y_f = y;

        // Sinon repositionne la carte.
        } else {
            entity->rect.x = x;
            entity->rect.y = y;
        }

    }

}





// Ajoute une carte à la main.
void handAddCard(Hand* hand, CardEntity* entity) {
    
    // Redimensionne la carte et l'ajoute à la liste.
    entity->rect.w = hand->card_rect.w;
    entity->rect.h = hand->card_rect.h;
    listAdd(hand->cards, entity);

    // Repositionne les cartes dans la main.
    handAddTransitions(hand, 30);

    // Repositionne la carte ajoutée.
    int x_f, y_f;
    getCardPosition(hand, &x_f, &y_f, listSize(hand->cards)-1);
    cardAddTransition(entity, createTransition(
        entity->rect.x, entity->rect.y, x_f, y_f, 30, sinusTransition
    ));
    
}



// Fonction de calcul de la position d'une carte à l'écran.
// id : l'id de la carte dans la liste.
void getCardPosition(Hand* hand, int* x, int* y, unsigned int id) {
    
    // Calcule l'ordonnée. Ajoute un offset si la carte est survolée.
    *y = hand->h - 0.7*hand->card_rect.h;
    if (hand->over == id) *y -= 0.3*hand->card_rect.h;

    // Calcule l'abcisse. Ajoute un offset si la carte est après celle survolée.
    *x = (hand->w - hand->card_rect.w*((listSize(hand->cards)+1)/2.0 - id))/2;
    if (id > hand->over) *x += hand->card_rect.w/2;

}
