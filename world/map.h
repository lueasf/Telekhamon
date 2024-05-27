#pragma once

#include "../graphics/tileset.h"
#include "../utils/list.h"





// Structure pour représenter une couche.
// map : données de la couche.
// w : largeur de la couche.
// h : hauteur de la couche.
struct s_Layer {
    unsigned int* map;
    unsigned int w;
    unsigned int h;
};
typedef struct s_Layer Layer;



// Structure pour représenter une carte.
// entity_height : nombre de couches sous les entités.
// l : nombre de couches.
// layers : liste des couches.
// tilesets : liste des tilesets des couches.
// x, yStairs : coordonnées de l'escalier de descente de l'étage.
struct s_Map {
    int entity_height;
    unsigned int l;
    Layer** layers;
    TileSet** tilesets;
    List* entities;
    int xStairs;
    int yStairs;
};
typedef struct s_Map Map;





// Constructeur et destructeur.
Layer* createLayer(unsigned int w, unsigned int h);
void freeLayer(Layer* map);





// Constructeur et destructeur.
Map* createMap(TileSet** tilesets, unsigned int l, int entity_height, unsigned int w, unsigned int h);
void freeMap(Map* map);





// Setter et getter pour Layer et Map.
unsigned int layerGet(Layer* layer, unsigned int x, unsigned int y);
void layerSet(Layer* layer, unsigned int x, unsigned int y, unsigned int id);
unsigned int mapGet(Map* map, unsigned int l, unsigned int x, unsigned int y);
void mapSet(Map* map, unsigned int l, unsigned int x, unsigned int y, unsigned int id);
