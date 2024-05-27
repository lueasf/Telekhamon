#pragma once

#include <stdlib.h>


// Structure pour explorer la map tile par tile en profondeur
// x, y = coordonnées de la tile à visiter. Vaut 424242 si c'est le fond de la pile.
// back = pointeur de la tile précédente dans la pile. Vaut NULL si c'est le fond de la pile.
struct s_Pile {
    int x;
    int y;
    struct s_Pile* back;
};
typedef struct s_Pile Pile;


// Crée une pile vide (back = NULL et x=y=424242) et la retourne
Pile* createEmptyPile();
void freePile(Pile* pile);



// Renvoie true ssi la pile est vide
int isEmptyPile(Pile* pile);


// Ajoute une tile à une pile, et renvoie l'adresse du nouveau haut de la pile
Pile* appendPile(Pile* pile, int x, int y);


// Free correctement la mémoire de la tile au dessus de la pile, et retourne le nouveau dessus de la pile
Pile* popPile(Pile* pile);

