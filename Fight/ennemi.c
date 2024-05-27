#include "ennemi.h"
#include "def_ennemi.h"
#include "casts.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>



//passives are dealt with when treating interactions that can potentially trigger passive effects

// function to create ennemi from id and difficulty modifier
Ennemi* CreateEnemi(SDL_Renderer* renderer, int id, int modifier){
    Ennemi* ennemi = from_id_ennemi(renderer,id,modifier);
    ennemi->transition = NULL;
    ennemi->nextSkill = NULL;
    ennemi->frame = 0;
    for (int i = 0; i < 8; i++) {
        ennemi->usedSkills[i] = 0;
    }
    return ennemi;
}

//function to apply skill effects 
void ApplySkill(Ennemi* caster, Skill* skill, int modifier, Player* player, List* team) {

    switch((*skill).target_type){
        case 0:
        Cast_Single(caster, skill, modifier, player, team);
        break;
        case 1:
        Cast_Self(caster, skill, modifier,team);
        break;
        case 2:
        Cast_All_Allies(caster, team,skill, modifier);
        break;
        case 3:
        Cast_All(caster, team, skill, modifier, player);
        break;
    }
}

void FreeEnnemi(Ennemi* ennemi, List* team) {
    
    // Supprime l'ennemi de la liste.
    if (team) {
        unsigned int i = listFind(team, ennemi);
        if (i < listSize(team)) listRemove(team, i); 
    }
    
    // Supprime l'ennemi.
    if (ennemi->transition) freeTransition(ennemi->transition);
    freeTileSet(ennemi->tileset);
    free(ennemi->name);
    free(ennemi->skill1);
    free(ennemi->skill2);
    free(ennemi->skill3);
    free(ennemi->skill4);
    free(ennemi);
    
}