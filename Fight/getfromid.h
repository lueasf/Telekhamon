#pragma once
#include "skills.h"
#include "passive.h"
#include <sqlite3.h>
#include <string.h>
#include "itoa.h"


//function to get ennemi name from ennemi id
char *from_id_name(int id);

//function to get ennemi max health from ennemi id
int from_id_health(int id,int modifier);

//function to get ennemi skill 1 from ennemi id
Skill* from_id_skill1(int id);

//function to get ennemi skill 2 from ennemi id
Skill* from_id_skill2(int id);

//function to get ennemi skill 3 from ennemi id
Skill* from_id_skill3(int id);

//function to get ennemi skill 4 from ennemi id (may be null)
Skill* from_id_skill4(int id);

