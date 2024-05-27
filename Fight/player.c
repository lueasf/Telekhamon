#include "skills.h"
#include "passive.h"
#include "init/init_cartes.h"
#include "player.h"


Player* CreatePlayer(char* name, int HP, int HPM, int energy, int energy_cap){
    Player* player = malloc(sizeof(Player));
    player->name = name;
    player->HP = HPM;
    player->MHP = HPM;
    player->energy= energy;
    player->energy_cap = energy_cap;
    for (int i = 0; i < 7; i++) player->status[i] = 0;
    for (unsigned int i = 0; i < NBCARTES; i++) player->deck[i] = 0;
    player->gold = 0;
    player->frame = 0;
    return player;

}

void freePlayer(Player* player){
    free(player);
}
