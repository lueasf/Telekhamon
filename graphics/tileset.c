#include "tileset.h"

#include <stdlib.h>
#include <SDL2/SDL_image.h>





// Constructeur de TileSet.
// path : chemin vers le fichier image du tileset au format PNG.
// w : largeur en nombre de tiles. 
// h : hauteur en nombre de tiles.
TileSet* createTileSet(SDL_Renderer* renderer, char* path, unsigned int w, unsigned int h) {
    if (w <= 0 || h <= 0) return NULL;

    // Charge la texture.
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors de la création du TileSet : %s", IMG_GetError());
        abort();
    }

    // Crée le TileSet.
    TileSet* tileset = (TileSet*)malloc(sizeof(TileSet));
    if (!tileset) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour TileSet");
        abort();
    }
    tileset->texture = texture;
    tileset->w = w;
    tileset->h = h;

    // Initialise le rect des tiles.
    SDL_QueryTexture(texture, NULL, NULL, &tileset->rect.w, &tileset->rect.h);
    tileset->rect.x = 0;
    tileset->rect.y = 0;
    tileset->rect.w /= w;
    tileset->rect.h /= h;

    // Renvoie le TileSet initialisé.
    return tileset;

}



// Destructeur de TileSet.
void freeTileSet(TileSet* tileset) {
    if (!tileset) return;
    SDL_DestroyTexture(tileset->texture);
    free(tileset);
}





// Renvoie le rect d'un tile.
void tileSetGetRect(TileSet* tileset, SDL_Rect* rect, unsigned int id) {
    rect->x = (id%tileset->w)*tileset->rect.w;
    rect->y = (id/tileset->w)*tileset->rect.h;
    rect->w = tileset->rect.w;
    rect->h = tileset->rect.h;
}
