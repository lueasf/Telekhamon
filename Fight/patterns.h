#include <math.h>
#include "skills.h"
#include "../utils/list.h"
#include "ennemi.h"
#include "player.h"



struct s_CombatStats {
    int momieHP;
    int momieMHP;
    int* momieEffects;

    int monstreHP;
    int monstreMHP;
    int* monstreEffects;

    float avgHPRatio;
    int alliesCount;
};
typedef struct s_CombatStats CombatStats;


// Gives a score to a skill given the current combat stats.
float computeScore(Skill* skill, CombatStats* stats);

// Returns whether or not a mob with the skill history usedSkills can use the skill i,
// which is, if using this skill doesn't create a 3-or-less-skills pattern that repeats 3 times in a row.
int canUseSkill(int i, int* usedSkills);

// Prend les paramètres actuals du combat
// et renvoie la capacité du monstre n°i à utiliser
Skill* selectSkill(List* enemies, int i, Player* momie);
