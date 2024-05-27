#pragma once
#include "stdlib.h"
#include "skills.h"
#include "ennemi.h"
#include "passive.h"
#include "getfromid.h"

//function to build an ennemi from its id taking in account the modifier (difficulty and stage)
Ennemi* from_id_ennemi(SDL_Renderer* renderer,int id,int modifier);
