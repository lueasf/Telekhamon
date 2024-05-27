#include "cartes.h"
#include "skills.h"

#include "getfromid.h"

#include <SDL2/SDL_image.h>
#include <stdlib.h>

#include "../graphics/font.h"



//constructeur
Carte* createCarte(SDL_Renderer* renderer, TTF_Font* font, const char* path, const char* info, 
    const char* name, int cout, Rare rare, int id, int exhaust) {
    Carte* carte = malloc(sizeof(Carte));

    carte->texture = IMG_LoadTexture(renderer, path);
    if (!carte->texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }
    SDL_SetTextureBlendMode(carte->texture, SDL_BLENDMODE_BLEND);

    carte->info = createTextureFromTextFromated(renderer, font, info);
    carte->name = name;
    carte->cout = cout;
    carte->rare = rare;
    carte->exhaust = exhaust;
    
    Skill* effects = from_id_skill1(id);
    carte->shield_t = effects->shield_t;
    carte->poison_t = effects->poison_t;
    carte->weakness_t = effects->weakness_t;
    carte->weak_t = effects->weak_t;
    carte->brittle_t = effects->brittle_t;
    carte->wounded_t = effects->wounded_t;
    carte->strength_t = effects->strength_t;
    carte->damage_t = effects->damage_t;
    carte->damage_tick_t = effects->damage_tick_t;
    carte->target_type = effects->target_type;
    carte->shield_s = effects->shield_s;
    carte->poison_s = effects->poison_s;
    carte->weakness_s = effects->weakness_s;
    carte->weak_s = effects->weak_s;
    carte->brittle_s = effects->brittle_s;
    carte->wounded_s = effects->wounded_s;
    carte->strength_s = effects->strength_s;
    carte->damage_s = effects->damage_s;
    carte->damage_tick_s = effects->damage_tick_s;
    free(effects);
    return carte;
}



void freeCarte(Carte* carte) {
    if (!carte) return;
    SDL_DestroyTexture(carte->texture);
    SDL_DestroyTexture(carte->info);
    free(carte);
}
