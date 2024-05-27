#include "damage.h"
#include <stdio.h>

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

//functions to deal damages from one target type to another

void Damage_Mob_Mob (Ennemi* caster,Ennemi* target,int damage, List* team){
    int new_HP;
    int new_shield;
    if (damage > 0){
    int total_damage= round((damage + (*caster).status[6] - (*caster).status[3])*(1-(0.33*MAX((*caster).status[2],1))*(0.33*MAX((*caster).status[4],1)))+(*caster).status[5]);
    int aftershield_damage = MAX(total_damage - (*caster).status[0],0);
    new_shield = MAX((*caster).status[0] - total_damage,0);
    new_HP = MAX((*caster).HP - aftershield_damage,0);
    
    
    if ((*caster).status[5] != 0){
        caster->status[5] = (*caster).status[5] -1;
    }}
    else {
        new_HP = MIN((*caster).HP - damage,(*caster).MHP);
        new_shield = (*caster).status[0];
    }
    
    
    caster->HP = new_HP;
    caster->status[0] = new_shield;
    return;
    
}

void Damage_Mob_Player(Ennemi* caster, Player* player, int damage){
    int total_damage= round((damage + (*caster).status[6] - (*player).status[3])*(1-(0.33*MAX((*caster).status[2],1))*(0.33*MAX((*player).status[4],1)))+(*player).status[5]);
    int aftershield_damage = MAX(total_damage - (*player).status[0],0);
    int new_shield = MAX((*player).status[0] - total_damage,0);
    int new_HP = MAX((*player).HP - aftershield_damage,0);
    if ((*player).status[5] != 0){
        player->status[5] = (*player).status[5] -1;
    }
    
        player->HP = new_HP;
        player->status[0] = new_shield;
        return;
    
}

void Damage_Player_Mob (Player* player, Ennemi* target, int damage, List* team) {
    int total_damage= round((damage + (*player).status[6] - (*target).status[3])*(1-(0.33*MAX((*player).status[2],1))*(0.33*MAX((*target).status[4],1)))+(*target).status[5]);
    int aftershield_damage = MAX(total_damage - (*target).status[0],0);
    int new_shield = MAX((*target).status[0] - total_damage,0);
    int new_HP = MAX((*target).HP - aftershield_damage,0);
    if ((*target).status[5] != 0){
        target->status[5] = (*target).status[5] -1;
    }
    
    target->HP = new_HP;
    target->status[0] = new_shield;
    return;
    
}

void Damage_Player_Player (Player* caster,Ennemi* target,int damage){
    int new_HP;
    int new_shield;
    if (damage > 0){
    int total_damage= round((damage + (*caster).status[6] - (*caster).status[3])*(1-(0.33*MAX((*caster).status[2],1))*(0.33*MAX((*caster).status[4],1)))+(*caster).status[5]);
    int aftershield_damage = MAX(total_damage - (*caster).status[0],0);
    new_shield = MAX((*caster).status[0] - total_damage,0);
    new_HP = MAX((*caster).HP - aftershield_damage,0);
        if ((*caster).status[5] != 0){
            caster->status[5] = (*caster).status[5] -1;
        }
    }
    else {
        new_HP = MIN((*caster).HP - damage,(*caster).MHP);
        new_shield = (*caster).status[0];
    }
    caster->HP = new_HP;
    caster->status[0] = new_shield;
    return;

}

void Poison_Damage_Player(Player* player){
    int new_HP = MAX((*player).HP - (*player).status[1],0);
    int new_poison = round(((*player).status[1]-1)/2);

    player->HP = new_HP;
    player->status[1] = new_poison;
    return;
    
}

void Poison_Damage_Mob(Ennemi* ennemi, List* team){
    int new_HP = MAX((*ennemi).HP - (*ennemi).status[1],0);
    int new_poison = round(((*ennemi).status[1]-1)/2);

    ennemi->HP = new_HP;
    ennemi->status[1] = new_poison;
    return;
    
}
