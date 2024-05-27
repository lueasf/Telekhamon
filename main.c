#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

#include "graphics/renderer.h"
#include "graphics/tileset.h"
#include "graphics/camera.h"
#include "graphics/font.h"
#include "world/map.h"
#include "world/mapgen.h"
#include "entities/level_entity.h"
#include "entities/Aetoile.h"
#include "utils/list.h"
#include "utils/stack.h"
#include "gui/fight_gui.h"
#include "gui/pause_gui.h"
#include "gui/main_menu.h"
#include "gui/deck_gui.h"
#include "gui/intro.h"
#include "Fight/cartes.h"
#include "Fight/init/init_cartes.h"
#include "Fight/player.h"
#include "game_state.h"



// Cap de FPS.
#define FPS_CAP 60




// Fonction pour initialiser la SDL.
void initSDL();





// Point d'entrée du programme.
int main(int argc, char* argv[]) {

    // Initialisation de la SDL;
    initSDL();

    // Initialiser la rng
    time_t t;
    srand((unsigned) time(&t));

    // Initialisation de la fenètre.
    int width = 1280;
    int height = 960;
    SDL_Window* window = SDL_CreateWindow(
        "SDL window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur d'initialisation de la fenêtre : %s", SDL_GetError());
        exit(-1);
    }

    // Initialisation du renderer.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in renderer init: %s", SDL_GetError());
        exit(-1);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Mise en place du fullscreen.
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    SDL_SetWindowDisplayMode(window, &mode);
    int fullscreen = 0;

    // Données de la boucle principale.
    GameState game_state = INTRO;
    GameState last_frame_game_state = INTRO;
    int run = 1;
    int pause = 0;

    // Polices d'écriture pour le rendu de textes.
    TTF_Font* font = createFont("GameData/fonts/ohmightyisis.ttf", 48);
    TTF_Font* big_font = createFont("GameData/fonts/ohmightyisis.ttf", 128);

    // Chargement des tilesets et de la caméra.
    TileSet* tileset = createTileSet(renderer, "GameData/textures/tilesets/pyramid.png", 6, 8);
    TileSet* tilesets[] = {tileset, tileset, tileset};
    TileSet* momie_tileset = createTileSet(renderer, "GameData/textures/entities/momie.png", 6, 8);
    TileSet* ennemi_map_tileset = createTileSet(renderer,"GameData/textures/entities/ennemi.png",3,1);
    Camera* camera = createCamera(0,0, width, height, 17);

    // Initialisation du joueur.
    Player* player = CreatePlayer("Player", 150,150,5,5);
    LevelEntity* entity = createLevelEntity(momie_tileset, 0, 1.2, 0,0);;
    float dist = 0.08f;
    int num_anim = 1;

    // Variables pour la map
    int map_floor = 0;
    Map* map = NULL;

    // Génération de la liste des cartes.
    Carte** cartes = initCards(renderer, font);

    // Variables pour l'audio
    // Liste des musiques de fond progressives
    Mix_Music** bgmlist = (Mix_Music**)malloc(6*sizeof(Mix_Music*));
    for (int k = 0; k < 6; k++) {
        char str[42];
        sprintf(str, "GameData/audio/sand_stage/%d.wav", k+1);
        bgmlist[k] = Mix_LoadMUS(str);
    }
    // Musiques de combat
    Mix_Music* fightIntro = Mix_LoadMUS("GameData/audio/enemy_rolled/enemy_rolled_intro.wav");
    Mix_Music* fightLoop = Mix_LoadMUS("GameData/audio/enemy_rolled/enemy_rolled_loop.wav");
    int introHasPlayed = 0;
    // Bruits de pas
    Mix_Chunk** stepList = (Mix_Chunk**)malloc(8*sizeof(Mix_Chunk*));
    for (int k = 0; k < 8; k++) {
        char str[42];
        sprintf(str, "GameData/audio/SFX/steps/step%d.wav", k+1);
        stepList[k] = Mix_LoadWAV(str);
    }
    int stepCounter = 0;
    // Bruit de capture d'ennemis
    Mix_Chunk* fightOverWav = Mix_LoadWAV("GameData/audio/SFX/enemyEncounter.wav");
    // Bruit de la pioche
    Mix_Chunk* drawCardWav = Mix_LoadWAV("GameData/audio/SFX/drawCard.wav");


    // Variables pour l'interface graphique.    
    FightGUI* fight_gui = createFightGUI(renderer, big_font, (int*)&game_state, width, height, player, cartes, drawCardWav);
    DeckGUI* deck_gui = createDeckGUI(renderer, big_font, cartes, &player->gold, player->deck, width, height);
    Intro* intro = createIntro(renderer, font, width, height);
    SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
    SDL_Color green = {0x00, 0xFF, 0x00, 0xFF};
    SDL_Texture* you_died = createTextureFromText(renderer, big_font, &red, "YOU DIED");
    SDL_Texture* you_win = createTextureFromText(renderer, big_font, &green, "CONGRATULATION");

    // Arguments constants à passer à la fonction pour lancer une run.
    void** argsStartRun = (void**)malloc(12*sizeof(void*)); 
    if (!argsStartRun) abort();
    argsStartRun[0] = &map;
    argsStartRun[1] = &player;
    argsStartRun[2] = &entity;
    argsStartRun[4] = deck_gui;
    argsStartRun[5] = tilesets;
    argsStartRun[6] = ennemi_map_tileset;
    argsStartRun[7] = momie_tileset;
    argsStartRun[8] = &width;
    argsStartRun[9] = &height;
    argsStartRun[10] = &game_state;
    argsStartRun[11] = renderer;
    MainMenu* main_menu = createMainMenu(renderer, big_font, width, height, &run, argsStartRun);
    PauseGUI* pause_gui = createPauseGUI(renderer, big_font, width, height, &pause, &game_state, (int*)&main_menu->state);
    pause_gui->main_volume = main_menu->volume;
    player->name = main_menu->box->text;

    // Variables directionnelles.
    int z = 0;
    int s = 0;
    int d = 0;
    int q = 0;

    // Timers.
    unsigned int last_anim = SDL_GetTicks();
    unsigned int last_frame = SDL_GetTicks();
    unsigned int last_frame_entity = SDL_GetTicks();

    // Boucle principale.
    int anim_k = 0;
    while (run) {

        // Cap le nombre d'exécution de la boucle à FPS_CAP/seconde.
        if (SDL_GetTicks()-last_frame < 1000.0/FPS_CAP) {
            SDL_Delay(1);
            continue;
        }
        last_frame = SDL_GetTicks();

        last_frame_game_state = game_state;

        // Booléens pour les touches.
        int left_click = 0;
        int right = 0;
        int left = 0;

        // Boucle des évènements.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            
            // Evènement de fermeture.
            case SDL_QUIT:
                run = 0;
                break;

            // Evènement de redimensionnement de la fenêtre.
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_GetWindowSize(window, &width, &height);
                    cameraSetScreenSize(camera, width, height);
                    fightGUISetScreenSize(fight_gui, width, height);
                    pauseGUISetScreenSize(pause_gui, width, height);
                    mainMenuSetScreenSize(main_menu, width, height);
                    deckGUISetScreenSize(deck_gui, width, height);
                    introSetStreenSize(intro, width, height);
                }
                break;
            
            // Touche pressée.
            case SDL_KEYDOWN: 
                if (event.key.keysym.sym != SDLK_F11) {
                    if (game_state == DEATH_SCREEN || game_state == END_SCREEN) {
                        game_state = MAIN_MENU;
                        continue;
                    } else if (game_state == MAIN_MENU) {
                        mainMenuProcessEvent(main_menu, renderer, event.key.keysym.sym);
                    } else if (game_state == INTRO) {
                        game_state = MAIN_MENU;
                    }
                }
                switch (event.key.keysym.sym) {

                    // Déplacement vers le bas
                    case SDLK_s: case SDLK_DOWN:
                        s=1;
                        break;

                    // déplacement vers la droite
                    case SDLK_d: case SDLK_RIGHT:
                        d=1;
                        break; 

                    // Déplacement à gauche
                    case SDLK_q: case SDLK_LEFT:
                        q=1;
                        break;

                    // Déplacement vers le haut
                    case SDLK_z: case SDLK_UP:
                        z=1;  
                        break;
                    }
                    break;
               
            // Touche relâchée.
            case SDL_KEYUP:
                if (game_state == MAIN_MENU) mainMenuKeyUp(main_menu);
                switch (event.key.keysym.sym) {
                
                // Toggle le fullscreen.
                case SDLK_F11:
                    fullscreen = !fullscreen;
                    SDL_SetWindowFullscreen(window, fullscreen);
                    cameraSetScreenSize(camera, mode.w, mode.h);
                    break;

                // Toggle la pause.
                case SDLK_ESCAPE:
                    if (game_state == MAIN_MENU) break;
                    pause = !pause;
                    if (pause) {
                        pause_gui->state = PGS_PAUSE;
                        pause_gui->volume->value = main_menu->volume->value;
                    } else {
                        main_menu->volume->value = pause_gui->volume->value;
                    }
                    break;

                // Toggle le menu du deck.
                case SDLK_TAB:
                    if (pause) break;
                    switch (game_state) {
                    case OVERWORLD:
                        game_state = DECK;
                        break;
                    case DECK:
                        game_state = OVERWORLD;
                        break;
                    default:
                        break;
                    }
                    break;

                // Repositionne le sprite en position statique.
                case SDLK_s: case SDLK_DOWN:
                    s=0;
                    break;   

                case SDLK_d: case SDLK_RIGHT:
                    d=0;
                    break;

                case SDLK_q: case SDLK_LEFT:
                    q=0;
                    break;

                case SDLK_z: case SDLK_UP:
                    z=0;
                    break;
                }
                break;

            // Click souris.
            case SDL_MOUSEBUTTONUP:
                if (game_state == DEATH_SCREEN || game_state == END_SCREEN) {
                    game_state = MAIN_MENU;
                    continue;
                } else if (game_state == INTRO) game_state = MAIN_MENU;
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    left_click = 1;
                    break;
                }
                break;

            // Click souris.
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    if (game_state == MAIN_MENU) mainMenuLeftClickDown(main_menu);
                    if (pause) pauseGUILeftClickDown(pause_gui);
                    break;
                }
                break;     

            // Molette souris.
            case SDL_MOUSEWHEEL:
                if(event.wheel.y > 0) {
                    if (game_state == DECK && deck_gui->state == DK_DECK)
                        deckGUIWheelUp(deck_gui);
                }
                else if(event.wheel.y < 0) {
                    if (game_state == DECK && deck_gui->state == DK_DECK)
                        deckGUIWheelDown(deck_gui);
                }
                break;

            } 
        
        }



        ///////////////////
        // UPDATE DU JEU //
        ///////////////////

        // Update en fonction de l'état du jeu.
        int m_x, m_y;
        SDL_GetMouseState(&m_x, &m_y);

        // Fin du jeu.
        if (game_state == OVERWORLD && map_floor == main_menu->level_number) {
            game_state = END_SCREEN;
        } 

        // Update si il y a pause.
        if (pause) {
            if (left_click) pauseGUILeftClick(pause_gui);
            pauseGUIUpdate(pause_gui, m_x, m_y);
            if (game_state == MAIN_MENU) pause = 0; 

        // Update hors de la pause.
        } else {
            switch (game_state) {
            case OVERWORLD:
                
                anim_k++;
                anim_k %= 6;
                num_anim=MomieMove(s,d,q,z,num_anim,entity,map,dist, anim_k);
                if (num_anim == 424242) { // code 424242 pour changer d'étage
                    num_anim = 6; // Reset l'animation

                    // Préparer le combat de boss de fin d'étage
                    prepareFight(fight_gui, createEnemyTeamBoss(renderer), main_menu->difficulty, map_floor, 1);
                    game_state = FIGHT;

                    // Préparer l'étage suivant
                    int xStairs = map->xStairs; int yStairs = map->yStairs;
                    freeMap(map);
                    map_floor += 1;
                    map = generateMap(map_floor, xStairs, yStairs, tilesets, entity, ennemi_map_tileset);
                    
                }
                if ((num_anim%6 == 1 || num_anim%6 == 4) && (num_anim/6)%2 == 1) { // La momie court et met un pied par terre ; joue un step
                    stepCounter = (stepCounter+1) % 8;
                    Mix_PlayChannel(-1, stepList[stepCounter], 0);
                }
                if(s==1||d==1||q==1||z==1){
                    last_frame_entity=last_frame;
                }
                
                // idle animations
                if (last_frame-last_frame_entity >1000.0/1) {
                    num_anim=((1+num_anim)%(2))+(entity->tileset->w)*((num_anim)/(entity->tileset->w));
                    levelEntitySetAnimation(entity,num_anim);
                    last_frame_entity=last_frame;
                }

                for (int i = listSize(map->entities)-1; i >= 0; i--) {
                    LevelEntity* enemy = (LevelEntity*)listGet(map->entities, i);
                    if (enemy == entity) continue;
                    levelEntityUpdatePath(enemy, entity, map);
                    levelEntityProcessMovement(enemy);
                    if (manhattanDistance(enemy->x, enemy->y, entity->x, entity->y) < 2) {
                        freeLevelEntity(enemy);
                        listRemove(map->entities, i);
                        prepareFight(fight_gui, createEnemyTeam(renderer), main_menu->difficulty, map_floor, 0);
                        game_state = FIGHT;
                        break;
                    }
                }

                // Mise à jour de la caméra et tri des entities.
                listSort(map->entities, cmpLevelEntities);
                cameraAtLevelEntity(camera,entity);
                break;

            // Update du combat.
            case FIGHT:
                if (left_click) fightGUILeftClick(fight_gui);
                fightGUIUpdate(fight_gui, m_x, m_y);
                break;

            // Update du menu principal.
            case MAIN_MENU:
                map_floor = 0;
                if (left_click) mainMenuLeftClick(main_menu);
                if (left) mainMenuLeft(main_menu);
                if (right) mainMenuRight(main_menu);
                mainMenuUpdate(renderer, main_menu, m_x, m_y);
                break;

            // Update du deck.
            case DECK:
                if (left_click) deckGUILeftClick(deck_gui); 
                deckGUIUpdate(renderer, deck_gui, m_x, m_y);
                break;

            // Update de l'intro.
            case INTRO:
                introUpdate(intro);
                if (introIsDone(intro)) game_state = MAIN_MENU;
                break;
            default:
                break;
            }

        }

        // Update de la musique
        if (game_state == OVERWORLD) {
            if (last_frame_game_state == FIGHT) {
                Mix_HaltMusic();
                Mix_PlayChannel(-1, fightOverWav, 0);
            }
            introHasPlayed = 0;
            if (!Mix_PlayingMusic()) {
                int bgmnb;
                if (map_floor >= 5) {
                    bgmnb = 5;
                } else {
                    bgmnb = map_floor;
                }
                Mix_PlayMusic(bgmlist[bgmnb], 0);
            }
        } else if (game_state == FIGHT) {
            if (last_frame_game_state != FIGHT) {
                Mix_HaltMusic();
                Mix_PlayChannel(-1, fightOverWav, 0);
            }
            if (!Mix_PlayingMusic()) {
                if (introHasPlayed) {
                    Mix_PlayMusic(fightLoop, 0);
                } else {
                    introHasPlayed = 1;
                    Mix_PlayMusic(fightIntro, 0);
                }
            }
        } else {
            introHasPlayed = 0;
            Mix_HaltMusic();
        }

    

        /////////////////////
        // RENDU GRAPHIQUE //
        /////////////////////

        // Nettoyage de l'écran.
        if (SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF) || SDL_RenderClear(renderer)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du nettoyage de l'écran : %s", SDL_GetError());
        }

        // Rendu en fonction de l'état du jeu.
        SDL_Rect rect;
        switch (game_state) {
        case OVERWORLD:
            mapRender(renderer, camera, map, 2);
            break;
        case FIGHT:
            fightGUIRender(renderer, fight_gui);
            break;
        case MAIN_MENU:
            mainMenuRender(renderer, main_menu);
            break;
        case DECK:
            deckGUIRender(renderer, deck_gui);
            break;
        case INTRO:
            introRender(renderer, intro);
            break;
        case DEATH_SCREEN:
            SDL_QueryTexture(you_died, NULL, NULL, &rect.w, &rect.h);
            rect.h = rect.h*width/rect.w/2;
            rect.w = width/2;
            rect.x = width/4;
            rect.y = (height-rect.h)/2;
            SDL_RenderCopy(renderer, you_died, NULL, &rect);
            break;
        case END_SCREEN:
            SDL_QueryTexture(you_win, NULL, NULL, &rect.w, &rect.h);
            rect.h = rect.h*width/rect.w/2;
            rect.w = width/2;
            rect.x = width/4;
            rect.y = (height-rect.h)/2;
            SDL_RenderCopy(renderer, you_win, NULL, &rect);
            break;
        }    

        // Rend le menu pause.
        if (pause) pauseGUIRender(renderer, pause_gui);
    
        // Switch les buffers.
        SDL_RenderPresent(renderer);
    }

    // Free des données audio.
    for (int k = 0; k < 6; k++) {
        Mix_FreeMusic(bgmlist[k]);
    }
    free(bgmlist);
    Mix_FreeMusic(fightIntro);
    Mix_FreeMusic(fightLoop);
    for (int k = 0; k < 8; k++) {
        Mix_FreeChunk(stepList[k]);
    }
    free(stepList);
    Mix_FreeChunk(fightOverWav);
    Mix_FreeChunk(drawCardWav);

    // Free des données du jeu.
    if (player != NULL) freePlayer(player);
    if (map != NULL) freeMap(map);
    freeCamera(camera);
    SDL_DestroyTexture(you_died);
    freeTileSet(ennemi_map_tileset);
    freeTileSet(momie_tileset);
    freeTileSet(tileset);
    freeCartes(cartes);
    freeFightGUI(fight_gui);
    freePauseGUI(pause_gui);
    freeDeckGUI(deck_gui);
    freeMainMenu(main_menu);
    freeIntro(intro);
    freeFont(big_font);
    freeFont(font);

    // Destruction du renderer et de la fenêtre.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return EXIT_SUCCESS;

}





// Fonction d'initialisation de la SDL.
void initSDL() {

    // Initialisation de la SDL.
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur d'initialisation de la SDL : %s", SDL_GetError());
        exit(-1);
    }
    atexit(SDL_Quit);

    // Initialisation de SDL_Image.
    int IMG_FLAGS = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(IMG_FLAGS) & IMG_FLAGS)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur d'initialisation de SDL_image : %s", IMG_GetError());
        exit(-1);
    }
    atexit(IMG_Quit);

    // Initialisation de SDL_Mix.
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur d'initialisation de SDL_mixer : %s", Mix_GetError());
        exit(-1);
    }
    atexit(Mix_Quit);

    // Initialisation de SDL_TTF.
    if (TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur d'initialisation de SDL_TTF : %s", TTF_GetError());
        exit(-1);
    }
    atexit(TTF_Quit);

}
