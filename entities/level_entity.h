#pragma once

#include "../graphics/tileset.h"
#include "../world/map.h"
#include "../utils/stack.h"
#include "../graphics/transition.h"





// Structure pour l'affichage d'entités dans une map.
struct s_LevelEntity {
    float x;
    float y;
    int id;
    float size;
    TileSet* tileset;
    unsigned int tile;
    Stack* moves;
    Transition* transition;
    long last_path_update;
};
typedef struct s_LevelEntity LevelEntity;



// Constructeur et destructeur.
LevelEntity* createLevelEntity(TileSet* tileset, int id,float size, float x, float y);
void freeLevelEntity(LevelEntity* entity);



// Fonctions de manipulation de l'animation de l'entité.
void levelEntityNextAnimStep(LevelEntity* entity);
void levelEntitySetAnimation(LevelEntity* entity, unsigned int n);
void levelEntitySetTile(LevelEntity* entity, unsigned int x, unsigned int y);
void levelEntitySetAnimationRow(LevelEntity* entity, unsigned int n);


// Fonctions de déplacement de l'entité. 
void levelEntityMove(LevelEntity* entity, float dx, float dy);
void levelEntitySetPosition(LevelEntity* entity, float x, float y);
void levelEntityTrack(LevelEntity* entity, LevelEntity* tracked, Map* map);
void levelEntityBrownian(LevelEntity* entity, Map* map);



// Ordre sur les entitées.
int cmpLevelEntities(const void* v1, const void* v2);

// Mouvement de la momie.
// Déplace la momie suivant la direction correspondant au nom de la fonction,
// Ne Déplace pas la momie s'il y a un mur dans la direction choisie,
// Et retourne le nouveau numéro de l'animation en cours : num_anim
int MomieMoveDown(LevelEntity* entity,int num_anim,Map* map,float dist, int k);
int MomieMoveRight(LevelEntity* entity,int num_anim,Map* map,float dist, int k);
int MomieMoveLeft(LevelEntity* entity,int num_anim,Map* map,float dist, int k);
int MomieMoveUp(LevelEntity* entity, int num_anim,Map* map,float dist, int k);
int MomieMove(int s,int d,int q,int z,int num_anim,LevelEntity* entity,Map* map,float dist, int k);

// Mouvement des ennemis.
void levelEntityUpdatePath(LevelEntity* entity, LevelEntity* tracked, Map* map);
void levelEntityProcessMovement(LevelEntity* entity);
