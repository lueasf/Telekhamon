#pragma once

#include <SDL2/SDL.h>
#include "../entities/level_entity.h"




// Déclaration de LevelEntity.
struct s_LevelEntity;
typedef struct s_LevelEntity LevelEntity;





// Structure pour modéliser une caméra.
// w, h : largeur et hauteur de l'écran.
// x, y : position de la caméra.
// width : largeur du champ de vison en tiles.
// height : hauteur du champ de vison en tiles.
struct s_Camera {
    unsigned int w;
    unsigned int h;
    float x;
    float y;
    float width;
    float height;
};
typedef struct s_Camera Camera;





// Constructeur et destructeur.
Camera* createCamera(float x, float y, unsigned int w, unsigned int h, float width);
void freeCamera(Camera* camera);



// Fonctions de manipulation de la caméra.
void cameraSetScreenSize(Camera* camera, unsigned int w, unsigned int h);
void cameraMove(Camera* camera, float dx, float dy);
void cameraSetWidth(Camera* camera, float width);
void cameraZoom(Camera* camera, float factor); 
void cameraAtLevelEntity(Camera* camera, LevelEntity* entity);



// Renvoie la taille d'un tile à l'écran.
void cameraGetTileSize(Camera* camera, float* tile_size);
