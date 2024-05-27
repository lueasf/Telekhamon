#pragma once

#include <SDL2/SDL.h>





// Structure pour modéliser un tileset.
// texture : image du tileset.
// rect : rectangle contenant une texture. 
// w : largeur en nombre de tiles. 
// h : hauteur en nombre de tiles.
struct s_TileSet {
    SDL_Texture* texture;
    SDL_Rect rect;
    unsigned int w;
    unsigned int h;
};
typedef struct s_TileSet TileSet;





// Constructeur et destructeur.
TileSet* createTileSet(SDL_Renderer* renderer, char* path, unsigned int w, unsigned int h);
void freeTileSet(TileSet* tileset);

// Fonctions de manipulation du tileset.
void tileSetGetRect(TileSet* tileset, SDL_Rect* rect, unsigned int id);
