#include "casts.h"



void Cast_Self(Ennemi* caster, Skill* skill, int modifier, List* team){
//cast on self only
    caster->status[0] = (*caster).status[0]+ (*skill).shield_s *(1+round(modifier/10));//apply shield effects
    caster->status[1] = (*caster).status[1]+ (*skill).poison_s *(1+round(modifier/10));//apply poison effects
    caster->status[2] = (*caster).status[2]+ (*skill).weakness_s *(1+round(modifier/10));//apply weakness effects
    caster->status[3] = (*caster).status[3]+ (*skill).weak_s *(1+round(modifier/10));//apply weak effects
    caster->status[4] = (*caster).status[4]+ (*skill).brittle_s *(1+round(modifier/10));//apply brittle effects
    caster->status[5] = (*caster).status[5]+ (*skill).wounded_s *(1+round(modifier/10));//apply wounded effects
    caster->status[6] = (*caster).status[6]+ (*skill).strength_s *(1+round(modifier/10));//aplly strength effects
    for (int i = 1 ; i <= (*skill).damage_tick_s ; i++){
        Damage_Mob_Mob(caster,caster, (*skill).damage_s,team);//deal damages X amount of times, with X number of ticks corresponding to skill
    }
}

void Cast_Single(Ennemi* caster, Skill* skill, int modifier, Player* player, List* team){
    //cast on player only
    caster->status[0] = (*caster).status[0]+ (*skill).shield_s *(1+round(modifier/10));//apply shield effects
    caster->status[1] = (*caster).status[1]+ (*skill).poison_s *(1+round(modifier/10));//apply poison effects
    caster->status[2] = (*caster).status[2]+ (*skill).weakness_s *(1+round(modifier/10));//apply weakness effects
    caster->status[3] = (*caster).status[3]+ (*skill).weak_s *(1+round(modifier/10));//apply weak effects
    caster->status[4] = (*caster).status[4]+ (*skill).brittle_s *(1+round(modifier/10));//apply brittle effects
    caster->status[5] = (*caster).status[5]+ (*skill).wounded_s *(1+round(modifier/10));//apply wounded effects
    caster->status[6] = (*caster).status[6]+ (*skill).strength_s *(1+round(modifier/10));//aplly strength effects
    for (int i = 1 ; i <= (*skill).damage_tick_s ; i++){
        Damage_Mob_Mob(caster,caster, (*skill).damage_s,team);//deal damages X amount of times, with X number of ticks corresponding to skill
    }
    player->status[0] = (*player).status[0]+ (*skill).shield_t *(1+round(modifier/10));//apply shield effects
    player->status[1] = (*player).status[1]+ (*skill).poison_t *(1+round(modifier/10));//apply poison effects
    player->status[2] = (*player).status[2]+ (*skill).weakness_t *(1+round(modifier/10));//apply weakness effects
    player->status[3] = (*player).status[3]+ (*skill).weak_t *(1+round(modifier/10));//apply weak effects
    player->status[4] = (*player).status[4]+ (*skill).brittle_t *(1+round(modifier/10));//apply brittle effects
    player->status[5] = (*player).status[5]+ (*skill).wounded_t *(1+round(modifier/10));//apply wounded effects
    player->status[6] = (*player).status[6]+ (*skill).strength_t *(1+round(modifier/10));//aplly strength effects
    for (int i = 1 ; i <= (*skill).damage_tick_t ; i++){
        Damage_Mob_Player(caster,player, (*skill).damage_t);//deal damages X amount of times, with X number of ticks corresponding to skill
    }
}

//*(team + i*sizeof(Ennemi))
void Cast_All_Allies(Ennemi* caster, List* team, Skill* skill, int modifier){
    caster->status[0] = (*caster).status[0]+ (*skill).shield_s *(1+round(modifier/10));//apply shield effects
    caster->status[1] = (*caster).status[1]+ (*skill).poison_s *(1+round(modifier/10));//apply poison effects
    caster->status[2] = (*caster).status[2]+ (*skill).weakness_s *(1+round(modifier/10));//apply weakness effects
    caster->status[3] = (*caster).status[3]+ (*skill).weak_s *(1+round(modifier/10));//apply weak effects
    caster->status[4] = (*caster).status[4]+ (*skill).brittle_s *(1+round(modifier/10));//apply brittle effects
    caster->status[5] = (*caster).status[5]+ (*skill).wounded_s *(1+round(modifier/10));//apply wounded effects
    caster->status[6] = (*caster).status[6]+ (*skill).strength_s *(1+round(modifier/10));//aplly strength effects
    for (int i = 1 ; i <= (*skill).damage_tick_s ; i++){
        Damage_Mob_Mob(caster,caster, (*skill).damage_s,team);//deal damages X amount of times, with X number of ticks corresponding to skill
    }
    for (int i = 0 ; i < listSize(team); i++){
        ((Ennemi*)listGet(team, i))->status[0] = ((Ennemi*)listGet(team, i))->status[0] + (*skill).shield_t * (1+round(modifier/10));//apply shield effects
        ((Ennemi*)listGet(team, i))->status[1] = ((Ennemi*)listGet(team, i))->status[1] + (*skill).poison_t * (1+round(modifier/10));//apply poison effects
        ((Ennemi*)listGet(team, i))->status[2] = ((Ennemi*)listGet(team, i))->status[2] + (*skill).weakness_t * (1+round(modifier/10));//apply weakness effects
        ((Ennemi*)listGet(team, i))->status[3] = ((Ennemi*)listGet(team, i))->status[3] + (*skill).weak_t * (1+round(modifier/10));//apply weak effects
        ((Ennemi*)listGet(team, i))->status[4] = ((Ennemi*)listGet(team, i))->status[4] + (*skill).brittle_t * (1+round(modifier/10));//apply brittle effects
        ((Ennemi*)listGet(team, i))->status[5] = ((Ennemi*)listGet(team, i))->status[5] + (*skill).wounded_t * (1+round(modifier/10));//apply wounded effects
        ((Ennemi*)listGet(team, i))->status[6] = ((Ennemi*)listGet(team, i))->status[6] + (*skill).strength_t * (1+round(modifier/10));//aplly strength effects
        for (int j = 1 ; j <= (*skill).damage_tick_t ; j++){
            Damage_Mob_Mob(caster,(listGet(team, i)), (*skill).damage_t,team);//deal damages X amount of times, with X number of ticks corresponding to skill
        }
    }
}

void Cast_All(Ennemi* caster, List* team, Skill* skill, int modifier,Player* player){
    caster->status[0] = (*caster).status[0]+ (*skill).shield_s *(1+round(modifier/10));//apply shield effects
    caster->status[1] = (*caster).status[1]+ (*skill).poison_s *(1+round(modifier/10));//apply poison effects
    caster->status[2] = (*caster).status[2]+ (*skill).weakness_s *(1+round(modifier/10));//apply weakness effects
    caster->status[3] = (*caster).status[3]+ (*skill).weak_s *(1+round(modifier/10));//apply weak effects
    caster->status[4] = (*caster).status[4]+ (*skill).brittle_s *(1+round(modifier/10));//apply brittle effects
    caster->status[5] = (*caster).status[5]+ (*skill).wounded_s *(1+round(modifier/10));//apply wounded effects
    caster->status[6] = (*caster).status[6]+ (*skill).strength_s *(1+round(modifier/10));//aplly strength effects
    for (int i = 1 ; i <= (*skill).damage_tick_s ; i++){
        Damage_Mob_Mob(caster,caster, (*skill).damage_s,team);//deal damages X amount of times, with X number of ticks corresponding to skill
    }
    for (int i = 0; i < listSize(team); i++){
        ((Ennemi*)listGet(team, i))->status[0] = ((Ennemi*)listGet(team, i))->status[0] + (*skill).shield_t * (1+round(modifier/10));//apply shield effects
        ((Ennemi*)listGet(team, i))->status[1] = ((Ennemi*)listGet(team, i))->status[1] + (*skill).poison_t * (1+round(modifier/10));//apply poison effects
        ((Ennemi*)listGet(team, i))->status[2] = ((Ennemi*)listGet(team, i))->status[2] + (*skill).weakness_t * (1+round(modifier/10));//apply weakness effects
        ((Ennemi*)listGet(team, i))->status[3] = ((Ennemi*)listGet(team, i))->status[3] + (*skill).weak_t * (1+round(modifier/10));//apply weak effects
        ((Ennemi*)listGet(team, i))->status[4] = ((Ennemi*)listGet(team, i))->status[4] + (*skill).brittle_t * (1+round(modifier/10));//apply brittle effects
        ((Ennemi*)listGet(team, i))->status[5] = ((Ennemi*)listGet(team, i))->status[5] + (*skill).wounded_t * (1+round(modifier/10));//apply wounded effects
        ((Ennemi*)listGet(team, i))->status[6] = ((Ennemi*)listGet(team, i))->status[6] + (*skill).strength_t * (1+round(modifier/10));//aplly strength effects
        for (int j = 1 ; j <= (*skill).damage_tick_t ; j++){
            Damage_Mob_Mob(caster,(listGet(team, i)), (*skill).damage_t,team);//deal damages X amount of times, with X number of ticks corresponding to skill
        }
    }
    player->status[0] = (*player).status[0]+ (*skill).shield_t *(1+round(modifier/10));//apply shield effects
    player->status[1] = (*player).status[1]+ (*skill).poison_t *(1+round(modifier/10));//apply poison effects
    player->status[2] = (*player).status[2]+ (*skill).weakness_t *(1+round(modifier/10));//apply weakness effects
    player->status[3] = (*player).status[3]+ (*skill).weak_t *(1+round(modifier/10));//apply weak effects
    player->status[4] = (*player).status[4]+ (*skill).brittle_t *(1+round(modifier/10));//apply brittle effects
    player->status[5] = (*player).status[5]+ (*skill).wounded_t *(1+round(modifier/10));//apply wounded effects
    player->status[6] = (*player).status[6]+ (*skill).strength_t *(1+round(modifier/10));//aplly strength effects
    for (int i = 1 ; i <= (*skill).damage_tick_t ; i++){
        Damage_Mob_Player(caster,player, (*skill).damage_t);//deal damages X amount of times, with X number of ticks corresponding to skill
    }
}
