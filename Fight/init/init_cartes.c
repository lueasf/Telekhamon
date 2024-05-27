#include "init_cartes.h"
#define NBCARTES 26
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../../graphics/font.h"

#include "../cartes.h"
#include <stdlib.h>





const char* names[NBCARTES] = {
    "Slash",
    "Guard",
    "Dry Skin",
    "Cursed Fog", 
    "Hidden Weapon",
    "Sacrifice",
    "Berserk",
    "Seth's Sandstorm",
    "Potent Poison",
    "Festering Wounds",
    "Poisonous Fog",
    "Assasination",
    "Betreyal",
    "Ra's Scorching Sun",
    "Oasis",
    "Horus's Reign",
    "Corrosive Mist",
    "Boosting Drug",
    "Forbidden Ritual",
    "Turtle Formation",
    "War Scream",
    "Lightning Strike",
    "Javelin Throw",
    "Sphynx Summoning",
    "Duat's Gate",
    "Serket's Ankh"
};

// Fillers à remplaçer.
const char* info[NBCARTES] = {
    "\eFFFF00Slash \e00FFFF (1) \n\eFFFFFFDeal \eFF0000 5 damages \eFFFFFF on \e00FF00 1 target \n\eFFFFFFJust a simple slash",
    "\eFFFF00Guard \e00FFFF (1) \n\eFFFFFFGives \e0000FF 5 sheilds \eFFFFFF to the \e00FF00 Player \n\eFFFFFFJust a basic sheild",
    "\eFFFF00Dry Skin \e00FFFF (1) \n\eFFFFFFGives \e0000FF 8 sheilds \eFFFFFF and \eFF0000 1 weak \eFFFFFF to the \e00FF00 Player \n\eFFFFFFMaybe use some skin cream ?",
    "\eFFFF00Cursed Fog \e00FFFF (2) \n\eFFFFFFApply \eFF0000 2 poison \eFFFFFF and \eFF0000 2 weak \eFFFFFF to \e00FF00 All targets \n\eFFFFFFYou shouldn't breath that...",
    "\eFFFF00Hidden Weapon \e00FFFF (0) \n\eFFFFFFDeal \eFF0000 3 damages \eFFFFFF and apply \eFF0000 2 weak \eFFFFFF to \e00FF00 1 target \n\eFFFFFF1 use per fight \nAre you a ninja ?",
    "\eFFFF00Sacrifice \e00FFFF (1) \n\eFFFFFFDeal \eFF0000 5 damages \eFFFFFF and apply \eFF0000 2 weak \eFFFFFF to \e00FF00 Everyone \n\eFFFFFFAre you, like, really sure about this ?",
    "\eFFFF00Berserk \e00FFFF (0) \n\eFFFFFFDeal \eFF0000 5 damages \eFFFFFF and gives \eFF0000 5 strengths \eFFFFFF to the \e00FF00 Player \n\eFFFFFF\"This Sword Is the Proof That I Have Lived\"",
    "\eFFFF00Seth's Sandstorm \e00FFFF (2) \n\eFFFFFFDeal \eFF0000 3 damages \eFFFFFF and apply \eFF0000 5 wounded \eFFFFFF, \eFF0000 3 brittles \eFFFFFF to \e00FF00 All target \n\eFFFFFF1 use per fight \nWeaker than Arrakis's Coriolis storms",
    "\eFFFF00Potent Poison \e00FFFF (1) \n\eFFFFFFApply \eFF0000 7 poison \eFFFFFF on \e00FF00 1 target \n\eFFFFFFThat would have been 700000 poison \nif this game was as bugged as Skyrim",
    "\eFFFF00Festering Wounds \e00FFFF (1) \n\eFFFFFFApply \eFF0000 4 wounded \eFFFFFF on \e00FF00 1 target \n\eFFFFFF1 use per fight \nPlease just go see a doctor",
    "\eFFFF00Poisonous Fog \e00FFFF (2) \n\eFFFFFFApply \eFF0000 4 poisons \eFFFFFF on \e00FF00 All targets \n\eFFFFFFIt's the same smell than Shrek !",
    "\eFFFF00Assasination \e00FFFF (1) \n\eFFFFFFDeal \eFF0000 10 damages \eFFFFFF on \e00FF00 1 target \n\eFFFFFF\"Everything is permitted, Nothing is true\"",
    "\eFFFF00Betreyal \e00FFFF (1) \n\eFFFFFFDeal \eFF0000 3 damages x3 \eFFFFFF on \e00FF00 1 target \n\eFFFFFF1 use per fight \nIs that you Yueh ?",
    "\eFFFF00Ra's Scorching Sun \e00FFFF (1) \n\eFFFFFFDeal \eFF0000 1 damages x6 \eFFFFFF on \e00FF00 All targets \n\eFFFFFFDid you take your sunglasses ?",
    "\eFFFF00Oasis \e00FFFF (1) \n\e00FF00 Heal 5PV \n\eFFFFFF\"Survival is the ability to swim in strange water.\"",
    "\eFFFF00Horus's Reign \e00FFFF (3) \n\e00FF00Heal 8PV \eFFFFFF, gives \eFF0000 3 strength \eFFFFFF to \e00FF00 Player \n\eFFFFFFDeals \eFF0000 3 damages \eFFFFFF and gives \eFF0000 3 wounded \eFFFFFF to \e00FF00 All targets \n\eFFFFFF1 use per fight \nIsn't that quiet overpowered ?",
    "\eFFFF00Corrosive Mist \e00FFFF (2) \eFFFFFF Apply \eFF0000 3 poison \eFFFFFF and \nremoves \e0000FF 5 shields \eFFFFFF to \e00FF00 Everyone \n\eFFFFFF1 use per fight \nIt's like swimming in chemical wastes",
    "\eFFFF00Boosting Drug \e00FFFF (0) \n\eFFFFFFGives \eFF0000 5 strength \eFFFFFF an \eFF0000 5 wounded \eFFFFFF to the \e00FF00 Player \n\eFFFFFF\"He who controls the spice controls the universe\"",
    "\eFFFF00Forbidden Ritual \e00FFFF (1) \n\eFFFFFFDeal \eFF0000 12 damages \eFFFFFF to the \e00FF00 Player \eFFFFFF, \n\eFF00008 damages \eFFFFFF , \eFF0000 3 wounded \eFFFFFF and \eFF0000 1 brittle \eFFFFFF to \e00FF00 All target \n\eFFFFFF1 use per fight \n\"Memento mori\"",
    "\eFFFF00Turtle Formation \e00FFFF (1) \n\eFFFFFFGives \e0000FF 8 shields \eFFFFFF to the \e00FF00 Player \n\eFFFFFFVeni, vidi, vici",
    "\eFFFF00War Scream \e00FFFF (0) \n\eFFFFFFGives \eFF0000 3 strengths \eFFFFFF to the \e00FF00 Player \n\eFFFFFFThis is Sparta !",
    "\eFFFF00Lightning Strike \e00FFFF (2) \n\eFFFFFFDeal \eFF0000 5 damages x3 \eFFFFFF on \e00FF00 1 target \n\eFFFFFFPower ! Unlimited power !",
    "\eFFFF00Javelin Throw \e00FFFF (1) \n\eFFFFFFDeal \eFF0000 8 damages \eFFFFFF and \eFF0000 2 wounded \eFFFFFF on \e00FF00 1 target \n\eFFFFFFI almost killed a teacher like that in high school",
    "\eFFFF00Sphynx Summoning \e00FFFF (2) \n\eFFFFFFDeal \eFF0000 18 damages \eFFFFFF to \e00FF00 1 target \n\eFFFFFFThat's an Egyptian Shai-Hulud, isn't that ?",
    "\eFFFF00Duat's Gate \e00FFFF (2) \n\eFFFFFFDeal \eFF0000 3 damages x5 \eFFFFFF and apply \eFF0000 3 wounded \eFFFFFF to \e00FF00 Everyone \n\eFFFFFF1 use per fight \nWell at least the enemies took damages",
    "\eFFFF00Serket's Ankh \e00FFFF (2) \n\eFFFFFFApply \eFF0000 20 poison \eFFFFFF on \e00FF00 1 target \n\eFFFFFFTake some Water of Life"
};

const char* path[NBCARTES] = {
    "GameData/textures/cards/Slash.jpg",
    "GameData/textures/cards/Guard.jpg",
    "GameData/textures/cards/Dry Skin.jpg",
    "GameData/textures/cards/Cursed Fog.jpg", 
    "GameData/textures/cards/Hidden Weapon.jpg",
    "GameData/textures/cards/Sacrifice.jpg",
    "GameData/textures/cards/Berserk.jpg",
    "GameData/textures/cards/Seth's Sandstorm.jpg",
    "GameData/textures/cards/Potent Poison.jpg",
    "GameData/textures/cards/Festering Wounds.jpg",
    "GameData/textures/cards/Poisonous Fog.jpg",
    "GameData/textures/cards/Assasination.jpg",
    "GameData/textures/cards/Betreyal.jpg",
    "GameData/textures/cards/Ra's Scorching Sun.jpg",
    "GameData/textures/cards/Oasis.jpg",
    "GameData/textures/cards/Horus's Reign.jpg",
    "GameData/textures/cards/Corrosive Mist.jpg",
    "GameData/textures/cards/Boosting Drug.jpg",
    "GameData/textures/cards/Forbidden Ritual.jpg",
    "GameData/textures/cards/Turtle Formation.jpg",
    "GameData/textures/cards/War Scream.jpg",
    "GameData/textures/cards/Lightning Strike.jpg",
    "GameData/textures/cards/Javelin Throw.jpg",
    "GameData/textures/cards/Sphynx Summoning.jpg",
    "GameData/textures/cards/Duat's Gate.jpg",
    "GameData/textures/cards/Serket's Ankh.jpg"
};

const int cost[NBCARTES] = {
    1,1,1,2,0,1,0,2,1,1,2,1,1,
    1,1,3,2,0,1,1,0,2,1,2,2,2
};

const int ids[NBCARTES] = {
    101,102,103,104,105,106,107,108,109,110,111,112,113,
    114,115,116,117,118,119,120,121,122,123,124,125,126
};

const Rare rarity[NBCARTES] = {
    COMMON,COMMON,COMMON,UNCOMMON,UNCOMMON,UNCOMMON,UNCOMMON,LEGENDARY,
    COMMON,RARE,UNCOMMON,UNCOMMON,UNCOMMON,LEGENDARY,COMMON,LEGENDARY,
    RARE,RARE,EPIC,COMMON,UNCOMMON,UNCOMMON,UNCOMMON,RARE,EPIC,EPIC
};

const int exhausts[NBCARTES] = {
    0,0,0,0,1,0,0,1,0,1,0,0,1,
    0,0,0,0,1,1,0,1,0,0,0,1,0
};





// INITIALISATION DES CARTES
Carte** initCards(SDL_Renderer* renderer, TTF_Font* font) {
    Carte** tab = (Carte**)calloc(NBCARTES, sizeof(Carte*));

    // Parcourt les cartes et les init avec les paramètres définis plus haut.
    for (int i = 0; i < NBCARTES; i++) {
        tab[i] = createCarte(
            renderer, font, 
            path[i] ,info[i], names[i], 
            cost[i], rarity[i], ids[i], exhausts[i]
        );
    }

    // Renvoie la liste initialisée.
    return tab;

}



// Destructeur des cartes.
void freeCartes(Carte** tab) {
    for(int i = 0; i < NBCARTES; i++){
        if (tab[i]) freeCarte(tab[i]);
    }
    free(tab);
}
