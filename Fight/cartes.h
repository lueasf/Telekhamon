#pragma once // empeche le fichier d'etre inclus pls fois dans le programme

#include "cartes.h"
#include "skills.h"


#include <SDL2/SDL_image.h>
#include <stdlib.h>

#include "../graphics/font.h"


// la cible de la carte est donné par le type Target
enum Target{
    SELF, // s'applique sur la momi, exemple : sort de guérison
    UN, // un seul ennenmi
    TOUS, // s'applique à tous les ennemis
    TOUS_SELF, // s'applique a la momi et aux ennemis
};
typedef enum Target Target;

// la rareté de la carte est donné par le type Rare
enum Rare{
    COMMON,
    UNCOMMON,
    EPIC,
    RARE,
    LEGENDARY,
    MYTHIC
};
typedef enum Rare Rare;

//structure de la carte

typedef struct {
    SDL_Texture* texture; //  image qu'on load
    SDL_Texture* info; // texte a voir quand on clique sur la carte
    const char* name; // nom de la carte
    int cout; // cout de la carte à l'energie du joueur
    int exhaust; // booléen idem
    int shield_t;//shield applied on target
    int poison_t;//poison applied on target
    int weakness_t;//weakness applied on target
    int weak_t;//weak applied on target
    int brittle_t;//brittle applied on target
    int wounded_t;//wounded  applied on target
    int strength_t;//strength applied on target
    int damage_t;//damages dealt to target
    int damage_tick_t;//number of times damages are dealt to target

    int target_type;//targetting type (0=player only, 1=self only, 2= all allies, 3= all)
    
    int shield_s;//shield applied on self
    int poison_s;//poison applied on self
    int weakness_s;//weakness applied on self
    int weak_s;//weak applied on self
    int brittle_s;//brittle applied on self
    int wounded_s;//wounded applied on self
    int strength_s;//strength applied on self
    int damage_s;//damages dealt to self
    int damage_tick_s;//number of times damages are dealt to self
    Rare rare;
} Carte;


Carte* createCarte(SDL_Renderer* renderer, TTF_Font* font, const char* path, const char* info,
    const char* name, int cout, Rare rare, int id, int exhaust);

void freeCarte(Carte* carte); //libérer une carte
