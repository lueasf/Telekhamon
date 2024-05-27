#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../utils/list.h"
#include "skills.h"
#include "player.h"
#include "../graphics/tileset.h"
#include "../graphics/transition.h"



// structure that represents ennemies
typedef struct{
    char *name; // ennemi name
    int HP; // ennemi's current HP
    int MHP; // ennemi's max HP
    int status[7]; //list of effects applied on ennemi
    int id; //ennemi's ID
    Skill* skill1;//ennemi's first skill
    Skill* skill2;//ennemi's second skill
    Skill* skill3;//ennemi's third skill
    Skill* skill4;//ennemi's fourth skill
    Skill* nextSkill;
    Passive* passive;//ennemi's passive
    SDL_Rect rect; 
    TileSet* tileset;
    Transition* transition;
    int frame;
    int last_hp;
    int teamslot;//ennemi's spot in the team
    int usedSkills[8];
}Ennemi;



Ennemi* CreateEnemi(SDL_Renderer* renderer, int id, int modifier);//create an ennemi from an id by taking in account the modifier

void ApplySkill(Ennemi* caster, Skill* skill ,int modifier, Player* player, List* team);//apply ennemi's skill

void FreeEnnemi(Ennemi* ennemi,List* team);
