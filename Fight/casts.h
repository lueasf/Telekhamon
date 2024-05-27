#pragma once
#include "ennemi.h"
#include "def_ennemi.h"
#include "skills.h"
#include "passive.h"
#include "damage.h"
#include "../utils/list.h"
#include <stdlib.h>
#include <math.h>

void Cast_Self(Ennemi* caster, Skill* skill, int modifier,List* team);

void Cast_Single(Ennemi* caster, Skill* skill, int modifier ,Player* player,List* team);

void Cast_All_Allies(Ennemi* caster,List* team, Skill* skill, int modifier);

void Cast_All(Ennemi* caster, List* team, Skill* skill, int modifier,Player* player);