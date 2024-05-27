#pragma once
#include "../cartes.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../../graphics/font.h"


#define NBCARTES 26



Carte** initCards(SDL_Renderer* renderer, TTF_Font* font);

void freeCartes(Carte** tab);
