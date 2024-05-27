#include "patterns.h"


// Gives a score to a skill given the current combat stats.
float computeScore(Skill* skill, CombatStats* stats) {
    int momieHP = stats->momieHP;
    int momieMHP = stats->momieMHP;
    float momieHPRatio = momieHP/momieMHP;
    int* momieEffects = stats->momieEffects;
    int monstreHP = stats->monstreHP;
    int monstreMHP = stats->monstreMHP;
    float monstreHPRatio = monstreHP/monstreMHP;
    int* monstreEffects = stats->monstreEffects;
    float avgHPRatio = stats->avgHPRatio;
    int alliesCount = stats->alliesCount;

    float score = 0;
    if (skill->target_type == 0) {
        score += skill->shield_t*(-3); // Ne shield pas son adversaire non mais
        score += skill->poison_t*(1 + momieHPRatio); // Poison la momie quand elle a beaucoup d'HP
        score += skill->weakness_t*(3 - 3*log(avgHPRatio) - 3*log(monstreHPRatio) - 3*((int)momieEffects[2]>=2)); // Effet broken + l'utilise particulièrement quand sa team est low sauf si ma momie est déjà weaknessée
        score += skill->weak_t*(1 - 3*log(avgHPRatio) - 3*log(monstreHPRatio)); // Rend la momie faible quand elle pourrait achever des alliés
        score += skill->brittle_t*(3 - 2*((int)momieEffects[4]>=2)); // Rend la momie brittle au maximum, sauf si elle l'est déjà pendant plus de deux tours
        score += skill->wounded_t*(3 + 2*momieEffects[5]); // Rend la momie wounded au maximum, surtout si elle a déjà des stacks
        score += skill->strength_t*(-3); // Ne rend pas son adversaire plus fort
        score += skill->damage_t*skill->damage_tick_t*(-30*log(momieHPRatio) + pow(momieHP, 0.3) - pow(monstreEffects[2], 2) - pow(monstreEffects[3], 1.5) + pow(momieEffects[4], 3) + pow(monstreEffects[6], 3)); // Attaque la momie par défaut si elle peut être achevée ce tour-là, si elle a beaucoup d'hp ou si le monstre a des effets positifs ou la momie des effets négatifs
        score += skill->damage_tick_t*(momieEffects[5]);
    } else if (skill->target_type == 1) {
        score += skill->shield_t*(-10*log(monstreHPRatio) + 0.2*(monstreMHP - monstreHP)); // Se shield quand peu d'HP
        score += skill->poison_t*(-2); // Ne veut pas se poison
        score += skill->weakness_t*(-3); // Ne veut pas perdre ses dégâts
        score += skill->weak_t*(-2); // Ne veut toujours pas perdre ses dégâts
        score += skill->brittle_t*(-3); // Ne veut surtout pas se faire casser en deux
        score += -(skill->wounded_t/2)*(skill->wounded_t + 1); // Evite de prendre des dégâts (somme des dégâts)
        score += skill->strength_t*(0.5*monstreHP + 5*log(monstreHPRatio) + -15*log(momieHPRatio)); // S'applique strength quand la momie est en mauvaise posture pour l'achever et quand le monstre en question n'est pas one-hit
        score += skill->damage_t*skill->damage_tick_t*(-1.3); // Evite de se mettre des dégâts
        score += skill->damage_tick_t*(-0.3*monstreEffects[5]); // Evite encore plus de se mettre des dégâts si le monstre a wounded
    } else if (skill->target_type == 2 || skill->target_type == 3) {
        score += alliesCount*skill->shield_t*(-10*log(avgHPRatio) + 0.2*(monstreMHP - monstreHP)); // Shield ses alliés quand peu d'HP
        score += alliesCount*skill->poison_t*(-5); // Ne veut pas poison ses alliés
        score += alliesCount*skill->weakness_t*(-5); // Ne veut pas perdre ses dégâts
        score += alliesCount*skill->weak_t*(-3); // Ne veut toujours pas perdre ses dégâts
        score += alliesCount*skill->brittle_t*(-5); // Ne veut surtout pas que sa team se fasse casser en deux
        score += -2*alliesCount*(skill->wounded_t/2)*(skill->wounded_t + 1); // Evite de donner des dégâts supplémentaires à son adversaire (somme des dégâts)
        score += alliesCount*skill->strength_t*(3*avgHPRatio + 5*log(monstreHPRatio) + -15*log(momieHPRatio)); // Applique strength aux alliés dès que possible et particulièrement quand la momie est en mauvaise posture pour l'achever
        score += alliesCount*skill->damage_t*skill->damage_tick_t*(-2); // Evite de mettre des dégâts à sa team, c'est super pas cool
        score += alliesCount*skill->damage_tick_t*(-0.3*monstreEffects[5]); // Evite encore plus de se mettre des dégâts si le monstre a wounded
    }

    score += 0.7*skill->shield_s*(-10*log(monstreHPRatio) + 0.2*(monstreMHP - monstreHP)); // Se shield quand peu d'HP
    score += 0.7*skill->poison_s*(-2); // Ne veut pas se poison
    score += 0.7*skill->weakness_s*(-3); // Ne veut pas perdre ses dégâts
    score += 0.7*skill->weak_s*(-2); // Ne veut toujours pas perdre ses dégâts
    score += 0.7*skill->brittle_s*(-3); // Ne veut surtout pas se faire casser en deux
    score += -0.7*(skill->wounded_s/2)*(skill->wounded_s + 1); // Evite de prendre des dégâts (somme des dégâts)
    score += 0.7*skill->strength_s*(0.5*monstreHP + 5*log(monstreHPRatio) + -15*log(momieHPRatio)); // S'applique strength quand la momie est en mauvaise posture pour l'achever et quand le monstre en question n'est pas one-hit
    score += 0.7*skill->damage_s*skill->damage_tick_s*(-1.3); // Evite de se mettre des dégâts
    score += 0.7*skill->damage_tick_s*(-0.3*monstreEffects[5]); // Evite encore plus de se mettre des dégâts si le monstre a wounded
    
    return score;
}


// Returns whether or not a mob with the skill history usedSkills can use the skill i,
// which is, if using this skill doesn't create a 3-or-less-skills pattern that repeats 3 times in a row.
int canUseSkill(int i, int* usedSkills) {
    int checkTaille1 = (int)(usedSkills[0] != i || usedSkills[1] != i);
    int checkTaille2 = (int)(
        usedSkills[0] != usedSkills[2] || usedSkills[0] != usedSkills[4]
     || usedSkills[1] != usedSkills[3] || usedSkills[1] != i
    );
    int checkTaille3 = (int)(
        usedSkills[1] != usedSkills[4] || usedSkills[1] != usedSkills[7]
     || usedSkills[0] != usedSkills[3] || usedSkills[0] != usedSkills[6]
     || usedSkills[2] != usedSkills[5] || usedSkills[2] != i
    );

    return (checkTaille1 && checkTaille2 && checkTaille3);
}

// Prend les paramètres actuals du combat
// et renvoie la capacité du monstre n°i à utiliser
Skill* selectSkill(List* enemies, int i, Player* momie) {
    // Récupère les données actualles du combat
    int momieHP = momie->HP;
    int momieMHP = momie->MHP;
    int* momieEffects = momie->status;

    int monstreHP = ((Ennemi*)listGet(enemies, i))->HP;
    int monstreMHP = ((Ennemi*)listGet(enemies, i))->MHP;
    int* monstreEffects = ((Ennemi*)listGet(enemies, i))->status;

    Skill* skill1 = ((Ennemi*)listGet(enemies, i))->skill1;
    Skill* skill2 = ((Ennemi*)listGet(enemies, i))->skill2;
    Skill* skill3 = ((Ennemi*)listGet(enemies, i))->skill3;
    
    int* usedSkills = ((Ennemi*)listGet(enemies, i))->usedSkills;

    float avgHPRatio = 0;
    int alliesCount = 0;
    for (int k = 0; k < listSize(enemies); k++) {
        Ennemi* enemy = listGet(enemies, k);
        alliesCount += 1;
        avgHPRatio += enemy->HP/enemy->MHP;
    }
    avgHPRatio /= alliesCount;

    // Prepare stats structure
    CombatStats* stats = (CombatStats*)malloc(sizeof(CombatStats));
    stats->momieHP = momieHP;
    stats->momieMHP = momieMHP;
    stats->momieEffects = momieEffects;
    stats->monstreHP = monstreHP;
    stats->monstreMHP = monstreMHP;
    stats->monstreEffects = monstreEffects;
    stats->avgHPRatio = avgHPRatio;
    stats->alliesCount = alliesCount;

    // Compute scores for each skill
    float score1 = computeScore(skill1, stats);
    float score2 = computeScore(skill2, stats);
    float score3 = computeScore(skill3, stats);

    int skillToUse;

    // Premier voeu
    if (score1 >= score2 && score1 >= score3) {
        if (canUseSkill(1, usedSkills)) {
            skillToUse = 1;
        }
    } else if (score2 >= score1 && score2 >= score3) {
        if (canUseSkill(2, usedSkills)) {
            skillToUse = 2;
        }
    } else if (score3 >= score1 && score3 >= score2) {
        if (canUseSkill(3, usedSkills)) {
            skillToUse = 3;
        }
    }

    // Second voeu
    else if (score1 >= score2 || score1 >= score3) {
        if (canUseSkill(1, usedSkills)) {
            skillToUse = 1;
        }
    } else if (score2 >= score1 || score2 >= score3) {
        if (canUseSkill(2, usedSkills)) {
            skillToUse = 2;
        }
    } else if (score3 >= score1 || score3 >= score2) {
        if (canUseSkill(3, usedSkills)) {
            skillToUse = 3;
        }
    }

    // Dernier voeu
    else if (canUseSkill(1, usedSkills)) {
        skillToUse = 1;
    } else if (canUseSkill(2, usedSkills)) {
        skillToUse = 2;
    } else {
        skillToUse = 3;
    }

    // Update usedSkills
    for (int j = 7; j > 0; j--) {
        usedSkills[j] = usedSkills[j-1];
    }
    usedSkills[0] = skillToUse;

    // Return decided skill
    if (skillToUse == 1) {
        return skill1;
    } else if (skillToUse == 2) {
        return skill2;
    } else {
        return skill3;
    }
    
}
