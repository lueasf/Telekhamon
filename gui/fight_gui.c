#include "fight_gui.h"

#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#include "../Fight/patterns.h"
#include "../game_state.h"
#include "../Fight/effects_tick.h"





#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))





// Met à jour la texture du label du tour.
void updateTurnLabel(FightGUI* gui) {
    char str[8] = {'T', 'U', 'R', 'N', ' ', '0'+gui->turn_number/10, '0'+gui->turn_number%10, 0};
    SDL_Color color = {0x00, 0x00, 0x00, 0xFF};
    gui->turn_label = createTextureFromText(gui->renderer, gui->font, &color, str);
}



// Fonction pour mettre fin au tour du joueur.
void endPlayerTurn(void** args) {
    FightGUI* gui = args[0];
    poison_player(gui->player);
    listClear(gui->enemies_list_for_turns);
    for (int i = 0 ; i < listSize(gui->enemies);i++){
        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
        listAdd(gui->enemies_list_for_turns, enemy);
        shield_ennemi(enemy);
        weakness_ennemi(enemy);
        weak_ennemi(enemy);
        brittle_ennemi(enemy);
        strength_ennemi(enemy);
    }
    gui->state = FS_ENNEMIES_TURN;
    gui->last_enemy_turn = SDL_GetTicks();
    gui->enemy_attack_done = 0;
    gui->current_enemy_turn = listGet(gui->enemies, 0);
}



// Met à jour les rects des ennemis.
void updatePlayerAndEnemiesRects(FightGUI* gui) {
    
    // Recalcule le rect du joueur. 
    gui->player_rect.w = 0.2*gui->hand->h;
    gui->player_rect.h = 0.2*gui->hand->h;
    gui->player_rect.y = gui->hand->h/5 + 0.6*gui->player_rect.h;
    gui->player_rect.x = gui->hand->h/6;

    // Recalcule la taille des ennemis.
    for (unsigned int i = 0; i < listSize(gui->enemies); i++) {
        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);

        // Si une transition existe, la met à jour.
        if (enemy->transition) {
            enemy->transition->x_f = gui->hand->w - 4*enemy->rect.w +
                1.2*((i%3)*enemy->rect.w + ((i > 2) ? enemy->rect.w/2 : 0));
            enemy->transition->y_f = 200 + 1.3*((i > 2) ? enemy->rect.h : 0);

        // Sinon les positionnent à leur place par défaut.
        } else {
            enemy->rect.w = 0.2*gui->hand->h;
            enemy->rect.h = 0.2*gui->hand->h;
            enemy->rect.x = gui->hand->w - 4*enemy->rect.w +
                1.2*((i%3)*enemy->rect.w + ((i > 2) ? enemy->rect.w/2 : 0));
            enemy->rect.y = gui->hand->h/5 + 1.3*((i > 2) ? enemy->rect.h : 0);
        }

    }

}



// Fonction pour tirer une carte.
int drawCard(FightGUI* gui) {

    // Calcul les poids pour tirer une carte aléatoire.
    int weight[NBCARTES] = {0};
    for (unsigned int i = 0; i < NBCARTES; i++) {
        weight[i] = gui->deck[i] + ((i > 0) ? weight[i-1] : 0);
    }
    
    // Remet les cartes utilisées dans la pioche.  
    if (!weight[NBCARTES-1]) {
        for (unsigned int i = 0; i < NBCARTES; i++) {
            gui->deck[i] = gui->used[i];
            weight[i] = gui->deck[i] + ((i > 0) ? weight[i-1] : 0);
            gui->used[i] = 0;
        }
        if (!weight[NBCARTES-1]) return -1;
    }

    // Génère le nombre aléatoire et renvoie la carte correspondante.
    int max = weight[NBCARTES-1];
    int num = rand()%max+1;
    for (unsigned int i = 0; i < NBCARTES; i++) {
        if (num <= weight[i]) return i;
    }
    return -1;

}





// Constructeur de FightGUI.
FightGUI* createFightGUI(SDL_Renderer* renderer, TTF_Font* font, int* game_state,
    unsigned int w, unsigned int h, Player* player, Carte** cards, Mix_Chunk* drawCardWav) {
    
    // Malloc l'interface.
    FightGUI* gui = (FightGUI*)malloc(sizeof(FightGUI));
    if (!gui) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour Hand");
        abort();
    }

    // Initialise les paramètres de l'interface.
    gui->game_state = game_state;
    gui->state = FS_PLAYER_TURN;
    gui->player = player;
    gui->current_enemy = NULL;
    gui->last_draw = 0;
    gui->turn_number = 1;
    gui->cards_to_draw = 0;
    gui->last_enemies_num = 0;
    gui->renderer = renderer;
    gui->cards = cards;
    gui->font = font;
    gui->enemy_attack_done = 0;
    gui->last_enemy_turn = 0;
    gui->player_tileset = createTileSet(renderer, "GameData/textures/entities/momie_fight.png", 4, 2);
    gui->effects_tileset = createTileSet(renderer, "GameData/textures/gui/effects.png", 3, 3);
    gui->isBossFight = 0;
    gui->drawCardWav = drawCardWav;
    updateTurnLabel(gui);

    // Crée la main et la liste des ennemis.
    gui->enemies = createList(5);
    gui->enemies_list_for_turns = createList(5);
    gui->hand = createHand(renderer, gui->used, 0.8, 1.5, w,h);
    
    // Charge la texture de l'arrière plan.
    gui->background = IMG_LoadTexture(renderer, "GameData/textures/gui/fight_background.jpeg");
    if (!gui->background) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }

    // Charge la texture du curseur.
    gui->cursor = IMG_LoadTexture(renderer, "GameData/textures/gui/cursor.png");
    if (!gui->cursor) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }

    // Charge la texture pour l'ombre.
    gui->shadow = IMG_LoadTexture(renderer, "GameData/textures/gui/circle.png");
    if (!gui->shadow) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }
    SDL_SetTextureBlendMode(gui->shadow, SDL_BLENDMODE_BLEND);

    // Charge la texture de la barre principale.
    gui->main_bar = IMG_LoadTexture(renderer, "GameData/textures/gui/fight_main_bar.png");
    if (!gui->main_bar) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }

    // Charge la texture d'une barre pleine.
    gui->bar_full = IMG_LoadTexture(renderer, "GameData/textures/gui/bar_full.png");
    if (!gui->bar_full) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }

    // Charge la texture d'une barre vide.
    gui->bar_empty = IMG_LoadTexture(renderer, "GameData/textures/gui/bar_empty.png");
    if (!gui->bar_empty) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du chargement de la texture : %s", IMG_GetError());
        abort();
    }

    // Zone pour le bouton de fin de tour.
    float area[8] = {
        0.2265625f, 0.09375f,
        0.7656250f, 0.09375f,
        0.6171875f, 0.87500f,
        0.3750000f, 0.87500f
    };

    // Crée le bouton de fin de tour.
    void** args0 = (void**)malloc(1*sizeof(void*));
    args0[0] = gui;
    gui->skip_button = createButton(
        renderer, font,
        "GameData/textures/gui/skip_button_in.png",
        "GameData/textures/gui/skip_button_out.png",
        area, NULL, args0, endPlayerTurn
    );

    // Initialise la taille de l'interface.
    fightGUISetScreenSize(gui, w,h);

    // Renvoie l'interface initialisée.
    return gui;

}



// Destructeur de FightGUI.
void freeFightGUI(FightGUI* gui) {
    SDL_DestroyTexture(gui->main_bar);
    SDL_DestroyTexture(gui->bar_full);
    SDL_DestroyTexture(gui->bar_empty);
    SDL_DestroyTexture(gui->cursor);
    SDL_DestroyTexture(gui->shadow);
    freeTileSet(gui->player_tileset);
    freeTileSet(gui->effects_tileset);
    freeButton(gui->skip_button);
    for (unsigned int i = 0; i < listSize(gui->enemies); i++) {
        FreeEnnemi((Ennemi*)gui->enemies->data[i], NULL);
    }
    freeList(gui->enemies);
    freeList(gui->enemies_list_for_turns);
    freeHand(gui->hand);
    free(gui);
}





// Fonction de préparation d'un combat.
// Lance un combat contre la team enemies.
// Le liste enemies est supprimée dans l'appel.
void prepareFight(FightGUI* gui, List* enemies, int difficulty, int stage, int isBossFight) {
    *gui->game_state = FIGHT;

    // Enregistre les ennemis du combat.
    for (unsigned int i = 0; i < listSize(gui->enemies); i++) {
        FreeEnnemi((Ennemi*)gui->enemies->data[i], gui->enemies);
    }
    listClear(gui->enemies);
    for (unsigned int i = 0; i < listSize(enemies); i++) {
        listAdd(gui->enemies, listGet(enemies, i));
    }
    freeList(enemies);
    
    // Initialise les ennemis et le joueur.
    updatePlayerAndEnemiesRects(gui);
    gui->last_enemies_num = listSize(gui->enemies);

    // Charge le deck.
    for (unsigned int i = 0; i < NBCARTES; i++) {
        gui->deck[i] = gui->player->deck[i];
        gui->used[i] = 0;
    }

    // Reset le status du joueur.
    for (int i = 0; i < 7; i++) {
        gui->player->status[i] = 0;
    }

    // Vide la main précédente.
    for (int i = 0; i < listSize(gui->hand->cards); i++) {
        freeCardEntity(listGet(gui->hand->cards, i));
    }
    listClear(gui->hand->cards);

    // Remet à zéro les paramètres du combat.
    gui->difficulty = difficulty;
    gui->stage = stage;
    gui->nb_enemies = listSize(gui->enemies);
    gui->current_enemy = listGet(gui->enemies, 0);
    gui->last_draw = 0;
    gui->turn_number = 1;
    gui->cards_to_draw = 0;
    gui->cards_to_draw = 6;
    gui->last_enemy_turn = 0;
    gui->enemy_attack_done = 0;
    gui->current_enemy_turn = listGet(gui->enemies, 0);
    gui->state = FS_DRAW_CARDS;
    gui->player->energy = gui->player->energy_cap;
    gui->isBossFight = isBossFight;
    updateTurnLabel(gui);

    // Choisis le premier skill des ennemis.
    for (unsigned int i = 0; i < listSize(gui->enemies); i++) {
        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
        enemy->nextSkill = selectSkill(gui->enemies, i, gui->player);
    }

}



// Renvoie une liste d'ennemis.
List* createEnemyTeam(SDL_Renderer* renderer) {
    List* team = createList(5);
    int n = 3 + rand()%3;
    for (unsigned int i = 0; i < n; i++) {
        listAdd(team, CreateEnemi(renderer, 1+rand()%7, 0));
    }
    return team;
}



// Renvoie une liste d'ennemis pour un boss.
List* createEnemyTeamBoss(SDL_Renderer* renderer) {
    List* team = createList(5);
    listAdd(team, CreateEnemi(renderer, 1+rand()%7, 0));
    listAdd(team, CreateEnemi(renderer, 11+rand()%3, 7));
    listAdd(team, CreateEnemi(renderer, 1+rand()%7, 0));
    return team;
}





// Fonction d'update de l'interface.
void fightGUIUpdate(FightGUI* gui, unsigned int m_x, unsigned int m_y) {
    
    // Si une carte est utilisée, supprime les ennemis morts.
    if (handUpdate(gui->hand, gui->player, gui->enemies, m_x, m_y)) {
        for (int i = listSize(gui->enemies)-1; i >= 0; i--) {
            Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
            if (enemy->HP <= 0) FreeEnnemi(enemy, gui->enemies);
        }
    }

    // Si un ennemi est tué.
    if (gui->last_enemies_num != listSize(gui->enemies)) {

        // Met à jour l'ennemi actuel.
        gui->last_enemies_num = listSize(gui->enemies);
        if (listFind(gui->enemies, gui->current_enemy) == listSize(gui->enemies)) {
            if (listSize(gui->enemies)) gui->current_enemy = (Ennemi*)listGet(gui->enemies, 0);
            else gui->current_enemy = NULL;
        }

        // Repositionne les ennemis restants.
        for (int i = 0; i < listSize(gui->enemies); i++) {
            Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);

            // Crée la transition des ennemis.
            if (enemy->transition) freeTransition(enemy->transition);
            enemy->transition = createTransition(
                enemy->rect.x, enemy->rect.y,
                gui->hand->w - 4*enemy->rect.w + 
                    1.2*((i%3)*enemy->rect.w + ((i > 2) ? enemy->rect.w/2 : 0)),
                gui->hand->h/5 + 1.3*((i > 2) ? enemy->rect.h : 0),
                30, linearTransition
            );

        }

    }

    // Met à jour la frame du joueur.
    if (gui->player->frame < 4) gui->player->frame = (SDL_GetTicks()/250)%4;

    // Effectue les transitions des ennemis.
    for (int i = 0; i < listSize(gui->enemies); i++) {
        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
        if (enemy->frame < 4) enemy->frame = (SDL_GetTicks()/250)%4;
        if (enemy->transition && enemy->transition->call(
            &enemy->rect.x, &enemy->rect.y, enemy->transition, NULL
        )) {
            freeTransition(enemy->transition);
            enemy->transition = NULL;
        }
    }

    // Effectue les updates spécifiques aux états du combat.
    switch (gui->state) {

    // Update du draw des cartes.
    case FS_DRAW_CARDS:
        if (gui->cards_to_draw > 0) {
            if (SDL_GetTicks() - gui->last_draw > 500) {
                gui->last_draw = SDL_GetTicks();
                int card = drawCard(gui);
                
                // Si une carte peut être piochée, l'ajoute.
                if (card >= 0) {
                    handAddCard(gui->hand, createCardEntity(gui->cards[card], card));

                    // Joue le son du piochage
                    Mix_PlayChannel(-1, gui->drawCardWav, 0);
                    gui->deck[card]--;
                }
                gui->cards_to_draw--;

            }

        // Passe au tour du joueur une fois le draw terminé.
        } else {
            shield_player(gui->player);
            weakness_player(gui->player);
            weak_player(gui->player);
            brittle_player(gui->player);
            strength_player(gui->player);
            gui->state = FS_PLAYER_TURN;
        }
        break;

    // Update des actions ennemies.
    case FS_ENNEMIES_TURN:
        if (SDL_GetTicks()-gui->last_enemy_turn > 300) {
            
            // Effectue l'attaque de l'ennemi.
            if (!gui->enemy_attack_done) {
                
                // Récupère les pvs de tout le monde.
                int last_hp = gui->player->HP;
                for (int i = 0; i < listSize(gui->enemies); i++) {
                    Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
                    enemy->last_hp = enemy->HP;
                } 

                // Applique le skill de l'ennemi.
                ApplySkill(
                    gui->current_enemy_turn, gui->current_enemy_turn->nextSkill, 
                    0, gui->player, gui->enemies
                );

                // Met à jour les animations de dégat.
                if (gui->player->HP != last_hp) gui->player->frame = 5;
                for (int i = 0; i < listSize(gui->enemies); i++) {
                    Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
                    if (enemy->last_hp != enemy->HP) enemy->frame = 5;
                } 

                // Met à jour l'animation d'attaque.
                gui->current_enemy_turn->frame = 4;

                // L'attaque est effectuée.
                gui->enemy_attack_done = 1;

            }

            // Après avoir attendu le temps de l'animation de dégats, passe à l'ennemi suivant.
            if (SDL_GetTicks()-gui->last_enemy_turn > 500) {
                gui->last_enemy_turn = SDL_GetTicks();
                gui->enemy_attack_done = 0;

                // Supprime les ennemis morts.
                for (int i = listSize(gui->enemies)-1; i >= 0; i--) {
                    Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
                    if (enemy->HP == 0) FreeEnnemi(enemy, gui->enemies);
                } 

                // Trouve le prochain ennemi en vie.
                int next = listFind(gui->enemies_list_for_turns, gui->current_enemy_turn);
                do {
                    next++;
                } while (
                    next < listSize(gui->enemies_list_for_turns) && 
                    listFind(gui->enemies, listGet(gui->enemies_list_for_turns, next)) == listSize(gui->enemies)
                );

                // Si les attaques ennemies sont finies, passe au tour du joueur.
                if (next >= listSize(gui->enemies_list_for_turns)) {

                    // Choisis le skill suivant des ennemis.
                    for (int i = 0; i < listSize(gui->enemies); i++) {
                        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
                        enemy->nextSkill = selectSkill(gui->enemies, i, gui->player);
                    }

                    // Diminution des buffs.
                    for (int i = listSize(gui->enemies)-1; i >= 0; i--){
                        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
                        poison_ennemi(enemy, gui->enemies);
                    }

                    // Supprime les ennemis morts.
                    for (int i = listSize(gui->enemies)-1; i >= 0; i--) {
                        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
                        if (enemy->HP == 0) FreeEnnemi(enemy, gui->enemies);
                    } 

                    // Prépare le pull des cartes et le tour suivant.
                    gui->cards_to_draw = min(4, 10-listSize(gui->hand->cards));
                    gui->state = FS_DRAW_CARDS;
                    gui->player->energy = min(gui->player->energy+3, gui->player->energy_cap); 
                    gui->turn_number++;
                    updateTurnLabel(gui); 

                // Sinon passe à l'ennemi suivant.
                } else gui->current_enemy_turn = listGet(gui->enemies_list_for_turns, next);
                
                // Reset les animations.
                gui->player->frame = (SDL_GetTicks()/250)%4;
                for (int i = 0; i < listSize(gui->enemies); i++) {
                    Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
                    enemy->frame = (SDL_GetTicks()/250)%4;
                }

            }
        }
        break;
    default:
        break;
    }

    // Retour au menu principal en cas de mort.
    if (gui->player->HP <= 0) *(gui->game_state) = DEATH_SCREEN;

    // Fin du combat en cas de victoire.
    if (listSize(gui->enemies) == 0) {
        if (gui->isBossFight) { // Reset les hp de la momie si elle a gagné un combat de boss
            gui->player->HP = gui->player->MHP;
        }
        *gui->game_state = OVERWORLD;
        gui->player->gold += gui->nb_enemies*50*pow(1.13, gui->difficulty+gui->stage+2);
    }

}



// Gestion du clic gauche.
void fightGUILeftClick(FightGUI* gui) {

    // Gestion du clic sur les cartes et les boutons.
    if (gui->state == FS_PLAYER_TURN) {
        onClick(gui->skip_button, gui->hand->m_x, gui->hand->m_y);
        if (!gui->hand->used) handLeftClick(gui->hand, gui->player, gui->current_enemy, gui->enemies, gui->drawCardWav);
    }

    // Gestion du clic sur les ennemis.
    for (unsigned int i = 0; i < listSize(gui->enemies); i++) {
        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);
        if (gui->hand->m_x >= enemy->rect.x && gui->hand->m_y >= enemy->rect.y &&
            gui->hand->m_x <= enemy->rect.x+enemy->rect.w && 
            gui->hand->m_y <= enemy->rect.y+enemy->rect.h) {
            gui->current_enemy = enemy;
        }
    }

}




// Fonction de redimensionnement de l'interface.
// w,h : dimension de l'écran.
void fightGUISetScreenSize(FightGUI* gui, unsigned int w, unsigned int h) {
    
    // Rendimensionne la main.
    handSetScreenSize(gui->hand, w,h);

    // Redimensionne le bouton de fin de tour.
    gui->skip_button->rect.h = 0.15f*h;
    gui->skip_button->rect.w = 2*gui->skip_button->rect.h;
    gui->skip_button->rect.x = (w-gui->skip_button->rect.w)/2;

    // Met à jour les rects des ennemis et du joueur.
    updatePlayerAndEnemiesRects(gui);

}
