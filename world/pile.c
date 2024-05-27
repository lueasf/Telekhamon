#include "pile.h"
#include <stdio.h>



// Crée une pile vide (back = NULL et x=y=424242) et la retourne
Pile* createEmptyPile() {
    Pile* pile = (Pile*)malloc(sizeof(Pile));
    if (pile == NULL) {
        printf("Couldn't allocate memory for the stack.");
        abort();
    }
    pile->x = 424242;
    pile->y = 424242;
    pile->back = NULL;

    return pile;
}



// Destructeur
void freePile(Pile* pile) {
    while (pile != NULL) {
        Pile* temp = pile->back;
        free(pile);
        pile = temp;
    }
}




// Renvoie true ssi la pile est vide
int isEmptyPile(Pile* pile) {
    return (pile->back == NULL && pile->x == 424242 && pile->y == 424242);
}


// Ajoute une tile à une pile, et renvoie l'adresse du nouveau haut de la pile
Pile* appendPile(Pile* pile, int x, int y) {
    Pile* newPile = (Pile*)malloc(sizeof(Pile));
    newPile->x = x;
    newPile->y = y;
    newPile->back = pile;

    return newPile;
}


// Free correctement la mémoire de la tile au dessus de la pile, et retourne le nouveau dessus de la pile
Pile* popPile(Pile* pile) {
    Pile* newPile = pile->back;
    free(pile);
    
    return newPile;
}
