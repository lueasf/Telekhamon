#pragma once
#include "ennemi.h"
#include "player.h"
#include "def_ennemi.h"
#include "skills.h"
#include "passive.h"
#include "damage.h"
#include "../utils/list.h"
#include "cartes.h"
#include <stdlib.h>
#include <math.h>



void Cast_Self_Card(Player* caster, Carte* skill, int modifier);

void Cast_Single_Card(Player* caster, Carte* skill, int modifier, Ennemi* player, List* team);

void Cast_All_Card(Player* caster, List* team, Carte* skill, int modifier,Player* player);

void ApplyCard(Player* caster, Carte* skill, int modifier, Ennemi* target, List* team);
