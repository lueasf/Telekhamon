#include "map.h"

#include "../entities/level_entity.h"

#include <stdlib.h>





// Constructeur de Layer.
// w,h : dimensions de la couche.
Layer* createLayer(unsigned int w, unsigned int h) {
    if (w <= 0 || h <= 0) return 0;
    Layer* layer = (Layer*)malloc(sizeof(Layer));
    if (!layer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Layer");
        abort();
    }
    
    // Initialise et renvoie une couche.
    layer->map = (unsigned int*)calloc(sizeof(unsigned int), w*h);
    if (!layer->map) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Layer->map");
        abort();
    }
    layer->w = w;
    layer->h = h;
    return layer;
}



// Destructeur de Layer.
void freeLayer(Layer* layer) {
    if (!layer) return;
    free(layer->map);
    free(layer);
}





// Constructeur de Map.
// tilesets : liste des tilesets par couche. Tableau de dimension l.
// l : nombre de couches.
// entity_height : nombre de couches a rendre sous les entités.
// w,h : dimensions des couche.
Map* createMap(TileSet** tilesets, unsigned int l, int entity_height, unsigned int w, unsigned int h) {
    if (l == 0 || w == 0 || h == 0) return NULL;

    // Initialise la map.
    Map* map = (Map*)malloc(sizeof(Map));
    if (!map) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Map");
        abort();
    }
    map->layers = (Layer**)malloc(sizeof(Layer*)*l);
    if (!map->layers) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Map->layers");
        abort();
    }
    map->tilesets = (TileSet**)malloc(sizeof(TileSet*)*l);
    if (!map->tilesets) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Map->tilesets");
        abort();
    }

    // Initialise les couches de la map et la renvoie.
    for (unsigned int i = 0; i < l; i++) {
        map->layers[i] = createLayer(w,h);
        map->tilesets[i] = tilesets[i];
    }
    map->entity_height = entity_height;
    map->entities = createList(32);
    map->l = l;
    return map;

}



// Destructeur de Map.
void freeMap(Map* map) {
    if (!map) return;
    
    for (unsigned int l = 0; l < map->l; l++) freeLayer(map->layers[l]);
    free(map->layers);
    
    for (unsigned int l = 0; l < listSize(map->entities); l++) {
        LevelEntity* entity = listGet(map->entities, l);
        if (entity->id != 0) freeLevelEntity(entity);
    }
    freeList(map->entities);

    free(map->tilesets);
    free(map);
}





unsigned int layerGet(Layer* layer, unsigned int x, unsigned int y) {
    return layer->map[y*layer->w+x];
}

void layerSet(Layer* layer, unsigned int x, unsigned int y, unsigned int id) {
    layer->map[y*layer->w+x] = id;
}

unsigned int mapGet(Map* map, unsigned int l, unsigned int x, unsigned int y) {
    return map->layers[l]->map[y*map->layers[l]->w+x];
}

void mapSet(Map* map, unsigned int l, unsigned int x, unsigned int y, unsigned int id) {
    map->layers[l]->map[y*map->layers[l]->w+x] = id;
}
