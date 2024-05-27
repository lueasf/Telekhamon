#pragma once

#include <SDL2/SDL.h>

#include "camera.h"
#include "tileset.h"
#include "../world/map.h"
#include "../entities/card_entity.h"
#include "../entities/level_entity.h"
#include "../utils/list.h"
#include "../gui/fight_gui.h"
#include "../gui/card_hand.h"
#include "../gui/pause_gui.h"
#include "../gui/deck_gui.h"
#include "../gui/main_menu.h"
#include "../gui/intro.h"





/*
    C’est à l’heure du commencement qu’il faut tout particulièrement veiller
à ce que les équilibres soient précis. Et cela, chaque sœur du Bene Gesserit 
le sait bien. Ainsi, pour entreprendre cette étude de la vie de Muad’Dib, 
il convient de le placer tout d’abord en son temps, en la cinquante-septième 
année de l’Empereur Padishah, Shaddam IV. Il convient aussi de bien le situer, 
sur la planète Arrakis. Et l’on ne devra pas se laisser abuser par le fait qu’il 
naquit sur Caladan et y vécut les quinze premières années de sa vie : Arrakis,
la planète connue sous le nom de Dune restera sienne à jamais. 
                -Extrait du "Manual de Muad’Dib" par la Princesse Irulan
*/





// Effectue le rendu d'une map.
void mapRender(SDL_Renderer* renderer, Camera* camera, Map* map, unsigned int n);



// Effectue le rendu d'une carte.
void cardRender(SDL_Renderer* renderer, CardEntity* card);



// Effectue le rendu des interfaces graphiques.
void fightGUIRender(SDL_Renderer* renderer, FightGUI* gui);
void pauseGUIRender(SDL_Renderer* renderer, PauseGUI* gui);
void deckGUIRender(SDL_Renderer* renderer, DeckGUI* gui);
void mainMenuRender(SDL_Renderer* renderer, MainMenu* gui);
void introRender(SDL_Renderer* renderer, Intro* intro);



// Effectue le rendu d'entités dans un niveau.
void levelEntityRender(SDL_Renderer* renderer, Camera* camera, LevelEntity* entity);
void levelEntitiesRender(SDL_Renderer* renderer, Camera* camera, List* entities);
