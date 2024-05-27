#include "level_entity.h"

#include "Aetoile.h"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>




// Constructeur de LevelEntity
// tileset : tileset qui contient les frames de l'animation de l'entité. Les
// lignes du tileset sont les animations et les colonnes les étapes de celles-ci.
// size : facteur multiplicatif sur la taille de l'entité. Elle sera affiché 
// avec une largeur fixée à size * taille d'un tile.
// x, y : position par défaut de l'entité.
LevelEntity* createLevelEntity(TileSet* tileset,int id,float size, float x, float y) {
    LevelEntity* entity = (LevelEntity*)malloc(sizeof(LevelEntity));
    if (!entity) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour LevelEntity");
        abort();
    }

    // Initialise l'entity et la renvoie.
    entity->moves = NULL;
    entity->transition = NULL;
    entity->tileset = tileset;
    entity->id = id;
    entity->size = size;
    entity->tile = 0;
    entity->last_path_update = 0;
    entity->x = x;
    entity->y = y;
    return entity;
    
}



// Destructeur de LevelEntity.
void freeLevelEntity(LevelEntity* entity) {
    if (!entity) return;
    if (entity->moves) {
        while (stackSize(entity->moves) > 0) freePoint((Point*)stackPop(entity->moves));
        freeStack(entity->moves);
    }
    if (entity->transition) freeTransition(entity->transition);
    free(entity);
}





// Passe à la frame suivante de l'animation actuelle.
void levelEntityNextAnimStep(LevelEntity* entity) {
    unsigned int anim = (entity->tile/entity->tileset->w)*entity->tileset->w;
    entity->tile = anim + (entity->tile-anim+1)%entity->tileset->w;
}



// Permet de définir l'animation à utiliser.
// n : id de l'animation à utiliser. Correspond au numéro de la tile.
void levelEntitySetAnimation(LevelEntity* entity, unsigned int n) {
    entity->tile = n;
}

// Permet de définir la ligne de l'animation à utiliser.
// n : id de l'animation à utiliser. Correspond à une ligne dans le tileset.
void levelEntitySetAnimationRow(LevelEntity* entity, unsigned int n) {
    entity->tile = n*entity->tileset->w;
}



// Permet de définir le tile à utiliser.
// x,y : coordonnées du tile dans le tileset.
void levelEntitySetTile(LevelEntity* entity, unsigned int x, unsigned int y) {
    entity->tile = y*entity->tileset->w+x;
}





// Permet de déplacer l'entité.
void levelEntityMove(LevelEntity* entity, float dx, float dy) {
    entity->x += dx;
    entity->y += dy;
}



// Permet de définir la position de l'entité.
void levelEntitySetPosition(LevelEntity* entity, float x, float y) {
    entity->x = x;
    entity->y = y;
}



// Permet de définir le chemin suivit par une entity pour en suivre une autre.
void levelEntityTrack(LevelEntity* entity, LevelEntity* tracked, Map* map) {

    // Supprime l'ancien chemin.
    if (entity->moves) {
        while (stackSize(entity->moves) > 0) freePoint((Point*)stackPop(entity->moves));
        freeStack(entity->moves);
    }

    // Calcule le nouveau avec A*.
    entity->moves = findPath(entity, tracked, map);

}



// Mouvement aléatoire.
void levelEntityBrownian(LevelEntity* entity, Map* map) {

    // Trouve un point libre autour de l'entité.
    Point pos = {(int)round(entity->x), (int)round(entity->y)};
    Point dest;
    do {
        dest.x = pos.x + rand()%3 - 1;
        dest.y = pos.y + rand()%3 - 1;
    } while ((dest.x == pos.x && dest.y == pos.y) || mapGet(map, 0, dest.x, dest.y) != 16);

    // Vide le stack.
    if (entity->moves) {
        while (stackSize(entity->moves) > 0) freePoint((Point*)stackPop(entity->moves));
    } else entity->moves = createStack();

    // Ajoute le nouveau point.
    stackPush(entity->moves, createPoint(dest.x, dest.y));

}





// Fonction de comparaison de coordonnées entre deux LevelEntity.
// Renvoie true si e1 doit être affiché avant e2.
int cmpLevelEntities(const void* v1, const void* v2) {
    LevelEntity* e1 = *(LevelEntity**)v1;
    LevelEntity* e2 = *(LevelEntity**)v2;

    // Compare les coordonnées.
    if (e1->y < e2->y || (e1->y == e2->y && e1->x < e2->x)) return -1;
    else if (e1->x == e2->x && e1->y == e2->y) return 0;
    return 1;

}

int MomieMoveDown(LevelEntity* entity, int num_anim, Map* map, float dist, int k){
    // Vérifie la présence d'un mur.
    int tileValue1 = mapGet(map,0,entity->x+0.2,entity->y+1.3);
    int tileValue2 = mapGet(map,0,entity->x+0.7,entity->y+1.3);
    if((tileValue1!=16 && tileValue1<36) || (tileValue2!=16 && tileValue2<36)) {
        levelEntitySetAnimation(entity,1);
        return 1;
    }
    // Vérifie la présence d'une tile d'un escalier de descente.
    if (tileValue1 >= 38 || tileValue2 >= 38) {
        levelEntitySetAnimation(entity,1);
        return 424242;
    }

    //update des sprites
    if (num_anim<2*entity->tileset->w-1){
        if (num_anim>=entity->tileset->w){
            if (k == 0) num_anim=1+(num_anim%entity->tileset->w)+1*entity->tileset->w;
            levelEntitySetAnimation(entity,num_anim);
                                    
        }
        else {
            levelEntitySetAnimationRow(entity,1);
            num_anim=1*entity->tileset->w;
        }
    }
    else{
        levelEntitySetAnimationRow(entity,1);
        num_anim=1*entity->tileset->w;
    }
    //déplacement des sprites
    levelEntityMove(entity,0,dist);
    entity->y=entity->y+dist;
    return num_anim;                        
}

int MomieMoveRight(LevelEntity* entity,int num_anim, Map* map,float dist, int k){
    // Vérifie la présence d'un mur
    int tileValue1 = mapGet(map,0,entity->x+1.1,entity->y+0.8);
    if(tileValue1!=16) {
        num_anim=2*entity->tileset->w;
        levelEntitySetAnimation(entity,num_anim);
        return num_anim;
    }
       
    //update des sprites
    if (num_anim<4*entity->tileset->w-1){
        if (num_anim>=3*entity->tileset->w){
            if (k == 0) num_anim=1+(num_anim%entity->tileset->w)+3*entity->tileset->w;
            levelEntitySetAnimation(entity,num_anim);
                                    
        }
        else {
            levelEntitySetAnimationRow(entity,3);
            num_anim=3*entity->tileset->w;
        }
    }
    else{
        levelEntitySetAnimationRow(entity,3);
        num_anim=3*entity->tileset->w;
        }
    //déplacement des sprites
    levelEntityMove(entity,dist,0);
    entity->x=entity->x+dist;
    return num_anim;
}

int MomieMoveLeft(LevelEntity* entity, int num_anim, Map* map,float dist, int k){
    // Vérifie la présence d'un mur
    int tileValue1 = mapGet(map,0,entity->x-0.2,entity->y+0.8); 
    if(tileValue1!=16) {
        num_anim=4*entity->tileset->w;
        levelEntitySetAnimation(entity,num_anim);
        return num_anim;
    }
    
    //update des sprites
    if (num_anim<6*entity->tileset->w-1){
        if (num_anim>=5*entity->tileset->w){
            if (k == 0) num_anim=1+(num_anim%entity->tileset->w)+5*entity->tileset->w;
            levelEntitySetAnimation(entity,num_anim);
                                    
        }
        else {
            levelEntitySetAnimationRow(entity,5);
            num_anim=5*entity->tileset->w;
        }
    }
    else{
        levelEntitySetAnimationRow(entity,5);
        num_anim=5*entity->tileset->w;
    }
    //déplacement des sprites
    levelEntityMove(entity,-dist,0);
    entity->x=entity->x-dist;
    return num_anim;
}

int MomieMoveUp(LevelEntity* entity, int num_anim, Map* map, float dist, int k){
    // Vérifie la présence d'un mur
    int tileValue1 = mapGet(map,0,entity->x+0.2,entity->y+0.2);
    int tileValue2 = mapGet(map,0,entity->x+0.7,entity->y+0.2);
    if((tileValue1!=16 && tileValue1<36) || tileValue2!=16 && tileValue1<36) {
        num_anim=6*entity->tileset->w;
        levelEntitySetAnimation(entity,num_anim);
        return num_anim;
    }
    
    //update des sprites
    if (num_anim<8*entity->tileset->w-1){
        if (num_anim>=7*entity->tileset->w){
            if (k == 0) num_anim=1+(num_anim%entity->tileset->w)+7*entity->tileset->w;
            levelEntitySetAnimation(entity,num_anim);
                                    
        }
        else {
            levelEntitySetAnimationRow(entity,7);
            num_anim=7*entity->tileset->w;
        }
    }
    else{
        levelEntitySetAnimationRow(entity,7);
        num_anim=7*entity->tileset->w;
    }
    //déplacement des sprites
    levelEntityMove(entity,0,-dist);
    entity->y=entity->y-dist;
    return num_anim;
}


// Fonction qui bouge la momie suivant les direction z,q,s,d pressée et retourne le nouveau num_anim
// Si jamais 3 ou plus directions sont pressées la momie ne bouge pas
int MomieMove(int s,int d,int q,int z,int num_anim,LevelEntity* entity, Map* map, float dist, int k){
    

    // Cas où la touche de déplacement n'est plus pressé.
    if(s==0&&d==0&&q==0&&z==0){   
        int l=(num_anim-num_anim%(entity->tileset->w))/(entity->tileset->w)+1;
       
        if(l%2==0){
            if (k == 0) num_anim=(l-2)*entity->tileset->w;
            levelEntitySetAnimation(entity,num_anim);
        }
        return num_anim;
    }
    if(s+q+d+z>=3||(s==1&&z==1)||(q==1&&d==1)){
        if((num_anim/entity->tileset->w)%2!=0){
            num_anim=(num_anim/(entity->tileset->w)-1)*(entity->tileset->w);
        }
        levelEntitySetAnimation(entity,num_anim);
        return num_anim;
    }
    if(s==1&&d==0&&q==0&&z==0){
        num_anim=MomieMoveDown(entity,num_anim,map,dist, k);
    }
    
    if(s==1&&d==1&&q==0&&z==0){
        int trash;

        if(num_anim/entity->tileset->w==1||mapGet(map,0,entity->x+1.1,entity->y+0.5)!=16 || mapGet(map,0,entity->x+1.1,entity->y+0.8)!=16){
            trash=MomieMoveRight(entity,num_anim,map,(float)dist/sqrt(2), k);
            num_anim=MomieMoveDown(entity,num_anim,map,(float)dist/sqrt(2), k);
        }
        else{
            trash=MomieMoveDown(entity,num_anim,map,(float)dist/sqrt(2), k);
            num_anim=MomieMoveRight(entity,num_anim,map,(float)dist/sqrt(2), k);
            if (trash == 424242) { // code 424242 pour changer d'étage
                num_anim = 424242;
            }
        }

    }

    if(s==1&&d==0&&q==1&&z==0){
        int trash;
        
        if(num_anim/entity->tileset->w==1||mapGet(map,0,entity->x-0.2,entity->y+0.8)!=16 || mapGet(map,0,entity->x-0.2,entity->y+0.8)!=16){
            trash=MomieMoveLeft(entity,num_anim,map,(float)dist/sqrt(2), k);
            num_anim=MomieMoveDown(entity,num_anim,map,(float)dist/sqrt(2), k);
        }
        else{
            trash=MomieMoveDown(entity,num_anim,map,(float)dist/sqrt(2), k);
            num_anim=MomieMoveLeft(entity,num_anim,map,(float)dist/sqrt(2), k);
            if (trash == 424242) { // code 424242 pour changer d'étage
                num_anim = 424242;
            }
        }
        
    }

    if(s==0&&d==1&&q==0&&z==0){
        num_anim=MomieMoveRight(entity,num_anim,map,dist, k);
    }
    
    if(s==0&&d==1&&q==0&&z==1){
        int trash;
        
        if(num_anim/entity->tileset->w==3||mapGet(map,0,entity->x+0.2,entity->y+0.2)!=16 || mapGet(map,0,entity->x+0.7,entity->y+0.2)!=16){
            trash=MomieMoveUp(entity,num_anim,map,(float)dist/sqrt(2), k);
            num_anim=MomieMoveRight(entity,num_anim,map,(float)dist/sqrt(2), k);
        }
        else{
            trash=MomieMoveRight(entity,num_anim,map,(float)dist/sqrt(2), k);
            num_anim=MomieMoveUp(entity,num_anim,map,(float)dist/sqrt(2), k);
        }
       
    }

    if(s==0&&d==0&&q==1&&z==0){
        num_anim=MomieMoveLeft(entity,num_anim,map,dist, k);
    }

    if(s==0&&d==0&&q==1&&z==1){
        int trash;
        
        if(num_anim/entity->tileset->w==5||mapGet(map,0,entity->x+0.2,entity->y+0.2)!=16 || mapGet(map,0,entity->x+0.7,entity->y+0.2)!=16){
            trash=MomieMoveUp(entity,num_anim,map,(float)dist/sqrt(2), k);
            num_anim=MomieMoveLeft(entity,num_anim,map,(float)dist/sqrt(2), k);
        }
        else{
            trash=MomieMoveLeft(entity,num_anim,map,(float)dist/sqrt(2), k);
            num_anim=MomieMoveUp(entity,num_anim,map,(float)dist/sqrt(2), k);
        }
        
    }
    
    if(s==0&&d==0&&q==0&&z==1){
        num_anim=MomieMoveUp(entity,num_anim,map,dist, k);  
    }
    
    return num_anim;
}





// Met à jour la traque des ennemis.
void levelEntityUpdatePath(LevelEntity* entity, LevelEntity* tracked, Map* map) {
    
    // Traque le jour si on est assez près.
    if (manhattanDistance(entity->x, entity->y, tracked->x, tracked->y) < 15) {
        if (SDL_GetTicks() - entity->last_path_update > 1000 || entity->moves == NULL) {
            entity->last_path_update = SDL_GetTicks(); 
            levelEntityTrack(entity, tracked, map);
        }
        
    // Effectue le mouvement brownien si l'ennemi est assez loin du joueur.
    } else {
        entity->last_path_update = 0;
        levelEntityBrownian(entity, map);
    }

}



// Fonction de transition des ennemis.
int enemyTransition(int* x, int* y, Transition* t, void* args) {
    if (t->k == t->n+1) return 1;

    // Met à jour la position de l'entité.
    LevelEntity* entity = (LevelEntity*)args;
    entity->x = t->x_i + t->k/(float)t->n * (t->x_f - t->x_i);
    entity->y = t->y_i + t->k/(float)t->n * (t->y_f - t->y_i);

    // Si on est à la fin de la transition, affecte directement les 
    // coordonnées pour éviter les erreurs d'arrondi.
    if (t->k == t->n) {
        entity->x = t->x_f;
        entity->y = t->y_f;
    }

    // Passe à l'étape suivante.
    t->k++;
    return 0;

}



// Process les positions enregistrées dans le stack de l'entity.
void levelEntityProcessMovement(LevelEntity* entity) {

    // Si il y a un mouvement dans le stack.
    if (!entity->transition && entity->moves && stackSize(entity->moves) > 0) {

        // Le récupère et crée la transition correspondante.
        Point* point = stackPop(entity->moves);
        entity->transition = createTransition(
            entity->x, entity->y,
            point->x, point->y,
            10, enemyTransition
        );
        entity->transition->k = 1;
        freePoint(point);

    }

    // Processs la transition en cours.
    if (entity->transition) {
        if (entity->transition->call(NULL, NULL, entity->transition, entity)) {
            freeTransition(entity->transition);
            entity->transition = NULL;
        }
    }

}
