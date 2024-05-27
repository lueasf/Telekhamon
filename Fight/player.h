#pragma once
#include "skills.h"
#include "passive.h"
#include "init/init_cartes.h"

// structure that represents player
typedef struct{
    char *name; // player's name
    int HP; // player's current HP
    int MHP; // player's max HP
    int status[7]; //list of effects applied on player
    int energy; //player's current energy
    int energy_cap; //player's maxximum energy
    int cards; //player's current amount of cards
    int hand_cap; //player's maximum amount of cards in hand
    int draw; //number of cards player draw each turn at most
    int gold;//player's curret currency
    int deck[NBCARTES];
    int frame;
}Player;


Player* CreatePlayer(char* name, int HP, int HPM, int energy, int energy_cap);

void freePlayer(Player* player);