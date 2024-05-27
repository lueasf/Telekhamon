#include "effects_tick.h"



void shield_player(Player* player){
    (*player).status[0] = 0;
    return ;
}

void poison_player(Player* player){
    Poison_Damage_Player(player);
    return;
}

void weakness_player(Player* player){
    if ((*player).status[2]>0){
        player->status[2] = (*player).status[2] -1;
    }
    return;
}

void weak_player(Player* player){
    if ((*player).status[3]>0){
        player->status[3] = (*player).status[3] -1;
    }
    return;
}

void brittle_player(Player* player){
    if ((*player).status[4]>0){
        player->status[4] = (*player).status[4] -1;
    }
    return;
}

void strength_player(Player* player){
    if ((*player).status[6]>0){
        player->status[6] = (*player).status[6] -1;
    }
    return;
}

void shield_ennemi(Ennemi* ennemi){
    (*ennemi).status[0] = 0;
    return ;
}

void poison_ennemi(Ennemi* ennemi, List* team){
    Poison_Damage_Mob(ennemi,team);
    return;
}

void weakness_ennemi(Ennemi* ennemi){
    if ((*ennemi).status[2]>0){
        ennemi->status[2] = (*ennemi).status[2] -1;
    }
    return;
}

void weak_ennemi(Ennemi* ennemi){
    if ((*ennemi).status[3]>0){
        ennemi->status[3] = (*ennemi).status[3] -1;
    }
    return;
}

void brittle_ennemi(Ennemi* ennemi){
    if ((*ennemi).status[4]>0){
        ennemi->status[4] = (*ennemi).status[4] -1;
    }
    return;
}

void strength_ennemi(Ennemi* ennemi){
    if ((*ennemi).status[6]>0){
        ennemi->status[6] = (*ennemi).status[6] -1;
    }
    return;
}