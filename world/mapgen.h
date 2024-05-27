#pragma once

#include <math.h>

#include "../graphics/tileset.h"
#include "tergen.h"
#include "map.h"




// Pi.
#define PI 3.141592654

#define OBJECT_TEXTURE_COUNT 5
#define ONE_OBJECT_OUT_OF 15


// Renvoie les lignes/colonnes min/max dans lequels il y a des valeurs. 
// Laise une colonne et une ligne d'écart entre le bort et les pièces.
// Hyp : le contenu de rooms ne touche pas les bords du tableau.
void getRoomsXYMinMax(Rooms* rooms, unsigned int* x_min, unsigned int* x_max,
    unsigned int* y_min, unsigned int* y_max);



// Renvoie la valeur d'une case de la map.
int getRoomsValueBounded(Rooms* rooms, unsigned int x, unsigned int y, 
    unsigned int x_min, unsigned int y_min);


// Décide des hyperparamètres de la génération de l'étage i et de la position de l'escalier de descente de l'étage précédent
HyperParam* getHyperParameters(int i, int xStairs, int yStairs);


// Fonction de coloriage d'une map à partir d'un Rooms.
// xStairs, yStairs = coordonnées de l'escalier de descente de l'étage précédent.
// tilesets : une liste de l tilesets, un pour chaque couche de la map.
// entity_height : nombre de couches sous les entités.
Map* generateMap(int floor, int xStairs, int yStairs, TileSet** tilesets, LevelEntity* momie, TileSet* ennemi_map_tileset);
