#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "../utils/list.h"
#include "../Fight/player.h"
#include "../Fight/ennemi.h"
#include "../Fight/init/init_cartes.h"
#include "../Fight/cartes.h"
#include "../graphics/tileset.h"
#include "card_hand.h"
#include "button.h"





// Structure pour l'état du combat.
typedef enum {
    FS_PLAYER_TURN,
    FS_ENNEMIES_TURN,
    FS_DRAW_CARDS
} FightState;





// Structure pour gérer l'interface de combat.
// hand : main du joueur.
// skip_button : bouton pour la fin de tour du joueur.
struct FightGUI {
    int* game_state;
    FightState state;
    TTF_Font* font;
    SDL_Renderer* renderer;
    SDL_Texture* cursor;
    SDL_Texture* shadow;
    SDL_Texture* turn_label;
    SDL_Texture* background;
    SDL_Texture* main_bar;
    SDL_Texture* bar_full;
    SDL_Texture* bar_empty;
    SDL_Texture* turn_texture;
    SDL_Texture* turn_number_texture;
    int turn_number;
    Ennemi* current_enemy;
    Hand* hand;
    Button* skip_button;
    Player* player;
    TileSet* player_tileset;
    TileSet* effects_tileset;
    SDL_Rect player_rect;
    List* enemies;
    List* enemies_list_for_turns;
    int last_enemies_num;
    int cards_to_draw;
    int deck[NBCARTES];
    int used[NBCARTES];
    long last_draw;
    long last_enemy_turn;
    int enemy_attack_done;
    Ennemi* current_enemy_turn;
    Carte** cards;
    int difficulty;
    int stage;
    int nb_enemies;
    int isBossFight;
    Mix_Chunk* drawCardWav;
};
typedef struct FightGUI FightGUI;





// Constructeur, destructeur.
FightGUI* createFightGUI(SDL_Renderer* renderer, TTF_Font* font, int* game_state, 
    unsigned int w, unsigned int h, Player* player, Carte** cards, Mix_Chunk* drawCardWav);
void freeFightGUI(FightGUI* gui);



// Fonctions de préparation d'un combat.
void prepareFight(FightGUI* gui, List* enemies, int difficulty, int stage, int isBossFight);
List* createEnemyTeam(SDL_Renderer* renderer);
List* createEnemyTeamBoss(SDL_Renderer* renderer);



// Gestion du clic gauche.
void fightGUIUpdate(FightGUI* gui, unsigned int m_x, unsigned int m_y);
void fightGUILeftClick(FightGUI* gui);



// Fonction de redimensionnement.
void fightGUISetScreenSize(FightGUI* gui, unsigned int w, unsigned int h);
