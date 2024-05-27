#pragma once
#include "ennemi.h"
#include "def_ennemi.h"
#include "skills.h"
#include "passive.h"
#include "player.h"
#include "../utils/list.h"
#include <stdlib.h>
#include <math.h>

//functions to deal damages from one target type to another

void Damage_Mob_Mob (Ennemi* caster,Ennemi* target,int damage, List* team);

void Damage_Mob_Player(Ennemi* caster, Player* player, int damage);

void Damage_Player_Mob (Player* player, Ennemi* target, int damage, List* team);

void Damage_Player_Player(Player* caster,Ennemi* target,int damage);

void Poison_Damage_Player(Player* player);

void Poison_Damage_Mob(Ennemi* ennemi, List* team);