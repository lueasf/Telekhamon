#include "camera.h"

#include <stdlib.h>





// Constructeur de Camera.
// x, y : position initiale de la caméra.
// w, h : largeur et hauteur de l'écran.
// width : largeur du champ de vision en tiles.
Camera* createCamera(float x, float y, unsigned int w, unsigned int h, float width) {
    if (w == 0 || width == 0) return NULL;
    Camera* camera = (Camera*)malloc(sizeof(Camera));
    if (!camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Camera");
        abort();
    }

    // Initialise camera et la renvoie.
    camera->w = w;
    camera->h = h;
    camera->x = x;
    camera->y = y;
    camera->width = width;
    camera->height = width*h/w;
    return camera;

}



// Destructeur de Camera.
void freeCamera(Camera* camera) {
    if (!camera) return;
    free(camera);
}





// Met à jour la caméra après un redimensionnement de l'écran.
// w : largeur de l'écran.
// h : hauteur de l'écran.
void cameraSetScreenSize(Camera* camera, unsigned int w, unsigned int h) {
    camera->w = w;
    camera->h = h;
    camera->height = camera->width*h/w;
}



// Fonction de déplacement de la caméra.
// dx, dy : déplacement de la caméra.
void cameraMove(Camera* camera, float dx, float dy) {
    camera->x += dx;
    camera->y += dy;
}



// Permet de définir la largeur du champ de vision.
// width : nouvelle largeur du champ de vision.
void cameraSetWidth(Camera* camera, float width) {
    camera->width = width;
    camera->height = width*camera->h/camera->w;
}



// Permet de zommer d'un facteur donné.
// factor : facteur multiplicatif du zoom.
void cameraZoom(Camera* camera, float factor) {
    camera->width *= factor;
    camera->height = camera->width*camera->h/camera->w;
}



// Permet à une carméra de suivre une entité.
void cameraAtLevelEntity(Camera* camera, LevelEntity* entity) {
    camera->x = entity->x;
    camera->y = entity->y;
}





// Calcule et renvoie la taille d'un tile à l'écran. 
void cameraGetTileSize(Camera* camera, float* tile_size) {
    *tile_size = camera->w/camera->width;
}
