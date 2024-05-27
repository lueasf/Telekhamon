#pragma once
#include "ennemi.h"
#include "def_ennemi.h"
#include "skills.h"
#include "passive.h"
#include "damage.h"
#include "player.h"
#include "../utils/list.h"
#include <stdlib.h>
#include <math.h>



void shield_player(Player* player);
void poison_player(Player* player);
void weakness_player(Player* player);
void weak_player(Player* player);
void brittle_player(Player* player);
void wounded_player(Player* player);
void strength_player(Player* player);

void shield_ennemi(Ennemi* ennemi);
void poison_ennemi(Ennemi* ennemi, List* team);
void weakness_ennemi(Ennemi* ennemi);
void weak_ennemi(Ennemi* ennemi);
void brittle_ennemi(Ennemi* ennemi);
void wounded_ennemi(Ennemi* ennemi);
void strength_ennemi(Ennemi* ennemi);