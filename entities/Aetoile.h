#pragma once

#include "../world/map.h"
#include "../utils/stack.h"
#include "level_entity.h"




// Structure pour un point.
struct s_Point {
    int x;
    int y;
};
typedef struct s_Point Point;



// Le noeud est une structure pour représenter une case de la map avec ses coordonnées et des 
// fonctions de calcul de chemin.
struct A_noeud{
    int x; // coord x de la case
    int y; // coord y de la case
    float g; // coût depuis la case de départ
    float h; // coût de la case jusqu'à la cible=joueur
    float f; // f = g + h est le cout total de départ jusqu'à la cible 
    struct A_noeud* parent; // noeud parent qui est la case précédente sur le chemin
};
typedef struct A_noeud A_noeud;



// Constructeur, destructeur.
Point* createPoint(int x, int y);
void freePoint(Point* point);


// constructeur
A_noeud* createnode(int x, int y, float g, float h, A_noeud* parent);

// destructeur
void destroynode(A_noeud* noeud);

float manhattanDistance(int x1, int y1, int x2, int y2);

Stack* findPath(LevelEntity* entity, LevelEntity* player, Map* map);
