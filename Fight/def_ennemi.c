#include "def_ennemi.h"

#include "../graphics/tileset.h"
#include "../graphics/font.h"
#include <SDL2/SDL_image.h>
#include <string.h>

#include <stdio.h>

//function to build an ennemi from its id taking in account the modifier (difficulty and stage)
Ennemi* from_id_ennemi(SDL_Renderer* renderer,int id,int modifier){
    Ennemi* ennemi = malloc(sizeof(Ennemi));
    ennemi->id = id;
    ennemi->name = from_id_name(id);
    ennemi->MHP = from_id_health(id,modifier);
    ennemi->HP = ennemi->MHP;
    
    //initialize effects stacks
    for (int i = 0; i<7; i++){
        ennemi->status[i] = 0;
    }
    
    //define ennemi skills
    ennemi->skill1 = from_id_skill1(id);
    ennemi->skill2 = from_id_skill2(id);
    ennemi->skill3 = from_id_skill3(id);
    ennemi->skill4 = from_id_skill4(id);
    
    // Charge la texture de l'ennemi.
    char buffer[128] = "GameData/textures/enemies/";
    ennemi->tileset = createTileSet(renderer, strcat(strcat(buffer, (*ennemi).name), ".png"), 4, 2);

    return ennemi;
}
