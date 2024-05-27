#include "renderer.h"

#include <sys/param.h>
#include <math.h>

#include "../Fight/cartes.h"
#include "../gui/textbox.h"
#include "../gui/button.h"





// Effectue le rendu d'un bouton.
void buttonRender(SDL_Renderer* renderer, Button* button, int x, int y) {
    
    // Copie la texture du bouton.
    if (inButton(button, x,y)) 
        SDL_RenderCopy(renderer, button->texture_in, NULL, &button->rect);
    else SDL_RenderCopy(renderer, button->texture_out, NULL, &button->rect);

    // Rend le texte du bouton.
    if (!button->text) return;
    SDL_Rect rect;
    SDL_QueryTexture(button->text, NULL, NULL, &rect.w, &rect.h);
    float ratio = 0.9*button->rect.h/(float)rect.h;
    rect.w *= ratio;
    rect.h *= ratio;
    rect.x = button->rect.x + (button->rect.w-rect.w)/2;
    rect.y = button->rect.y + (1.1*button->rect.h-rect.h)/2;
    SDL_RenderCopy(renderer, button->text, NULL, &rect);

}



// Effectue le rendu d'une boite de texte.
void textBoxRender(SDL_Renderer* renderer, TextBox* box) {

    // Rend le fond de la boite.
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &box->rect);

    // Rend le texte si il y en a un.
    if (box->text_texture) SDL_RenderCopy(renderer, box->text_texture, &box->image_rect, &box->render_rect);

    // Rend le curseur.
    SDL_Rect cursor_rect = {box->cursor_position, box->rect.y+2, 2, box->rect.h-4};
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &cursor_rect);

}



// Permet le rendu d'une infobulle de carte.
void renderInfo(SDL_Renderer* renderer, CardEntity* entity, unsigned int w, unsigned int h, 
    unsigned int m_x, unsigned int m_y, float alpha) {

    // Récupère la taille des infobulles.
    SDL_Rect rect = {0,0,0,0};
    SDL_QueryTexture(entity->carte->info, NULL, NULL, &rect.w, &rect.h);
    
    // En calcule la taille et rend un fond semi-transparent.
    rect.w *= w/1500.0;
    rect.h *= w/1500.0;
    rect.y = (m_y+rect.h > h) ? m_y - rect.h : m_y;
    rect.x = (m_x+rect.w > w) ? m_x - rect.w : m_x;
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xA0*alpha); 
    SDL_RenderFillRect(renderer, &rect);

    // Rend le texte de l'infobulle.
    rect.w -= 20;
    rect.h -= 20;
    rect.x += 10;
    rect.y += 10;        
    SDL_SetTextureAlphaMod(entity->carte->info, 255*alpha); 
    SDL_RenderCopy(renderer, entity->carte->info, NULL, &rect);
    SDL_SetTextureAlphaMod(entity->carte->info, 255); 

}



// Effectue le rendu d'une main.
void handRender(SDL_Renderer* renderer, Hand* hand) {

    // Parcourt et rend les cartes et la carte utilisée.
    for (unsigned int i = 0; i < hand->cards->size; i++) {
        if (hand->used_id == i) cardRender(renderer, hand->used);
        CardEntity* entity = (CardEntity*)hand->cards->data[i];
        cardRender(renderer, entity);
    }

    // Rend la carte utilisée si elle est en fin de main.
    if (hand->used_id == hand->cards->size) cardRender(renderer, hand->used);

    // Rend les infobulles sur les cartes.
    if (hand->over != -1 && hand->info_alpha > 0.01f) {
        renderInfo(
            renderer, (CardEntity*)hand->cards->data[hand->over], 
            hand->w, hand->h, hand->m_x, hand->m_y, hand->info_alpha
        );
    }

}





// Fonction de rendu d'une map.
// renderer : le renderer de la fenètre.
// camera : la caméra du niveau.
// tileset : le tileset à utiliser pour le rendu. Hyp : max(map->map) < nb tiles dans tileset.
// map : la map dont on veut faire le rendu.
void mapRender(SDL_Renderer* renderer, Camera* camera, Map* map, unsigned int n) {
    
    // Récupère les dimensions d'un tile à l'écran.
    float tile_size;
    cameraGetTileSize(camera, &tile_size);
    
    // En déduit le rect du tile.
    SDL_Rect cam_rect = {0,0,(int)tile_size+1,(int)tile_size+1};

    // Calcule la position du centre de la caméra.
    float center_x = (tile_size)*(camera->width/2.0 - camera->x - 0.5f);
    float center_y = (tile_size)*(camera->height/2.0 - camera->y - 0.5f);
    
    // Calcule la partie de la map à rendre.
    float max_x = MIN(map->layers[0]->w, camera->x+camera->width/2+0.5);
    float min_x = MAX(0, camera->x-camera->width/2+0.5);
    float max_y = MIN(map->layers[0]->h, camera->y+camera->height/2+0.5);
    float min_y = MAX(0, camera->y-camera->height/2+0.5);
    
    // Parcourt la map et la rend.
    SDL_Rect tile_rect;
    for (unsigned int l = 0; l < map->l; l++) {
        
        // Rend au besoin les entitées.
        if (l == map->entity_height) levelEntitiesRender(renderer, camera, map->entities);
        
        // Rend une couche.
        for (unsigned int i = min_x; i < max_x; i++) {
            for (unsigned int j = min_y; j < max_y; j++) {

                // Récupère l'id d'un tile.
                unsigned int id = mapGet(map, l, i,j);
                if (!id) continue;

                // Rend un tile.
                cam_rect.x = tile_size*i + center_x;
                cam_rect.y = tile_size*j + center_y;
                tileSetGetRect(map->tilesets[l], &tile_rect, id);
                SDL_RenderCopy(renderer, map->tilesets[l]->texture, &tile_rect, &cam_rect);

            }
        }
    }
    
    // Rend si besoin les entitées au dessus de toutes les couches de la map.
    if (map->entity_height >= map->l) levelEntitiesRender(renderer, camera, map->entities);

}





// Effectue le rendu d'une carte.
void cardRender(SDL_Renderer* renderer, CardEntity* card) {

    // Rend le template.
    SDL_SetTextureAlphaMod(card->carte->texture, 255*card->alpha); 
    SDL_RenderCopy(renderer, card->carte->texture, NULL, &card->rect);
    SDL_SetTextureAlphaMod(card->carte->texture, 255); 

}





// Effectue le rendu de l'interface de combat.
void fightGUIRender(SDL_Renderer* renderer, FightGUI* gui) {

    // Rend l'arrière plan.
    SDL_Rect main_bar_rect = {0,0, gui->hand->w, gui->skip_button->rect.h/2};
    SDL_Rect background_rect = {0, main_bar_rect.h, main_bar_rect.w, gui->hand->h-main_bar_rect.h};
    SDL_RenderCopy(renderer, gui->background, NULL, &background_rect);

    // Rend l'ombre sous le joueur.
    SDL_Rect player_shadow_rect = {
        gui->player_rect.x,gui->player_rect.y+7*gui->player_rect.h/8, 
        gui->player_rect.w, gui->player_rect.h/4
    };
    SDL_SetTextureColorMod(gui->shadow, 0, 0, 0);
    SDL_SetTextureAlphaMod(gui->shadow, 127); 
    SDL_RenderCopy(renderer, gui->shadow, NULL, &player_shadow_rect);
    SDL_SetTextureAlphaMod(gui->shadow, 255); 
    SDL_SetTextureColorMod(gui->shadow, 255, 255, 255);

    // Rend le joueur.
    SDL_Rect texture_rect;
    tileSetGetRect(gui->player_tileset, &texture_rect, gui->player->frame);
    SDL_RenderCopy(renderer, gui->player_tileset->texture, &texture_rect, &gui->player_rect);
   
    // Rend les icones des effets des ennemis.
    int k = 0;
    for (int i = 0; i < 7; i++) {
        if (!gui->player->status[i]) continue;
        
        // Calcul le rect de l'effet.
        SDL_Rect effect_rect = {
            0 + k*(main_bar_rect.h), main_bar_rect.h,
            main_bar_rect.h, main_bar_rect.h
        };

        // Affiche l'icone autant de fois que de stack d'effets.
        for (int j = 0; j < gui->player->status[i]; j++) {
            SDL_Rect effect_img_rect;
            tileSetGetRect(gui->effects_tileset, &effect_img_rect, i);
            SDL_RenderCopy(renderer, gui->effects_tileset->texture, &effect_img_rect, &effect_rect);
            effect_rect.y += (main_bar_rect.h)/3;
        }
        k++;

    }

    // Rend les ennemis.
    for (int i = 0; i < listSize(gui->enemies); i++) {
        Ennemi* enemy = (Ennemi*)listGet(gui->enemies, i);

        // Rend l'ombre sous l'ennemi.
        SDL_Rect shadow_rect = {enemy->rect.x,enemy->rect.y+7*enemy->rect.h/8, enemy->rect.w, enemy->rect.h/4};
        SDL_SetTextureColorMod(gui->shadow, 0, 0, 0);
        SDL_SetTextureAlphaMod(gui->shadow, 127); 
        SDL_RenderCopy(renderer, gui->shadow, NULL, &shadow_rect);
        SDL_SetTextureAlphaMod(gui->shadow, 255); 
        SDL_SetTextureColorMod(gui->shadow, 255, 255, 255);

        // Rend l'ennemi.
        SDL_Rect texture_rect;
        tileSetGetRect(enemy->tileset, &texture_rect, enemy->frame);
        SDL_RenderCopy(renderer, enemy->tileset->texture, &texture_rect, &enemy->rect);

        // Rend la bare de vie de l'ennemi.
        SDL_Rect enemy_pv = {enemy->rect.x, enemy->rect.y-enemy->rect.h/10,enemy->rect.w,enemy->rect.h/10};
        SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xFF);
        SDL_RenderFillRect(renderer, &enemy_pv);
        enemy_pv.x+=2; enemy_pv.y+=2;
        enemy_pv.w-=4; enemy_pv.h-=4;
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xD0);
        SDL_RenderFillRect(renderer, &enemy_pv);
        enemy_pv.w *= enemy->HP/(float)enemy->MHP;
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &enemy_pv);
 
        // Rend les icones des effets des ennemis.
        int k = 0;
        for (int i = 0; i < 7; i++) {
            if (!enemy->status[i]) continue;
            
            // Calcul le rect de l'effet.
            SDL_Rect effect_rect = {
                enemy_pv.x-2 + k*(enemy_pv.h+2), enemy_pv.y+enemy_pv.h+2,
                enemy_pv.h+2, enemy_pv.h+2
            };

            // Affiche l'icone autant de fois que de stack d'effets.
            for (int j = 0; j < enemy->status[i]; j++) {
                SDL_Rect effect_img_rect;
                tileSetGetRect(gui->effects_tileset, &effect_img_rect, i);
                SDL_RenderCopy(renderer, gui->effects_tileset->texture, &effect_img_rect, &effect_rect);
                effect_rect.y += (enemy_pv.h+2)/2;
            }
            k++;

        }

        // Rend le curseur.
        if (gui->current_enemy == enemy) {
            texture_rect.w = enemy->rect.w/6;
            texture_rect.h = enemy->rect.h/6;
            texture_rect.x = enemy->rect.x + (enemy->rect.w-texture_rect.w)/2;
            texture_rect.y = enemy->rect.y - (2+0.1*sin(SDL_GetTicks()/250.0))*texture_rect.h;
            SDL_RenderCopy(renderer, gui->cursor, NULL, &texture_rect);
        }

    }

    // Rend la barre du haut.
    SDL_RenderCopy(renderer, gui->main_bar, NULL, &main_bar_rect);

    // Calcule les rects pour la barre de pv.
    float pv_ratio = gui->player->HP/(float)gui->player->MHP;
    float ep_ratio = gui->player->energy/(float)gui->player->energy_cap;
    SDL_Rect rect_full_img = {0,0,0,0};
    SDL_QueryTexture(gui->bar_full, NULL, NULL, &rect_full_img.w, &rect_full_img.h);
    rect_full_img.x = (1.0-pv_ratio)*rect_full_img.w;
    rect_full_img.w -= rect_full_img.x;
    SDL_Rect rect_empty = {
        gui->skip_button->rect.x-0.20*main_bar_rect.w, 0.1*main_bar_rect.h, 
        0.20*main_bar_rect.w, 0.8*main_bar_rect.h
    };
    SDL_Rect rect_full = {
        rect_empty.x+rect_empty.w*(1.0-pv_ratio), rect_empty.y,
        rect_empty.w-rect_empty.w*(1.0-pv_ratio), rect_empty.h
    };

    // Rend la barre de pv.
    SDL_SetTextureColorMod(gui->bar_full, 186, 161, 86);
    SDL_RenderCopy(renderer, gui->bar_full, NULL, &rect_empty);
    SDL_SetTextureColorMod(gui->bar_full, 200, 20, 40);
    SDL_RenderCopy(renderer, gui->bar_full, &rect_full_img, &rect_full);
    SDL_SetTextureColorMod(gui->bar_full, 255, 255, 255);
    SDL_RenderCopy(renderer, gui->bar_empty, NULL, &rect_empty);

    // Rend le numéro du tour.
    SDL_Rect turn_rect;
    SDL_QueryTexture(gui->turn_label, NULL, NULL, &turn_rect.w, &turn_rect.h);
    float ratio = turn_rect.w/(float)turn_rect.h;
    turn_rect.h = 0.8*main_bar_rect.h;
    turn_rect.w = turn_rect.h*ratio;
    turn_rect.x = (rect_empty.x-turn_rect.w)/2;
    turn_rect.y = 0.1*main_bar_rect.h;
    SDL_RenderCopy(renderer, gui->turn_label, NULL, &turn_rect);

    // Calcule les rects pour la barre de ep et la rend.
    SDL_Point point = {gui->hand->w/2, 0};
    SDL_Rect ep_rect_empty = {gui->skip_button->rect.x};
    rect_empty.x = gui->skip_button->rect.x + gui->skip_button->rect.w;
    rect_full.x = gui->skip_button->rect.x + gui->skip_button->rect.w;
    rect_full.w = rect_empty.w-rect_empty.w*(1.0-ep_ratio);
    SDL_QueryTexture(gui->bar_full, NULL, NULL, &rect_full_img.w, &rect_full_img.h);
    rect_full_img.x = (1.0-ep_ratio)*rect_full_img.w;
    rect_full_img.w -= rect_full_img.x;

    // Rend l'ui.
    SDL_SetTextureColorMod(gui->bar_full, 186, 161, 86);
    SDL_RenderCopyEx(renderer, gui->bar_full, NULL, &rect_empty, 0, &point, SDL_FLIP_HORIZONTAL);
    SDL_SetTextureColorMod(gui->bar_full, 20, 40, 200);
    SDL_RenderCopyEx(renderer, gui->bar_full, &rect_full_img, &rect_full, 0, &point, SDL_FLIP_HORIZONTAL);
    SDL_SetTextureColorMod(gui->bar_full, 255, 255, 255);
    SDL_RenderCopyEx(renderer, gui->bar_empty, NULL, &rect_empty, 0, &point, SDL_FLIP_HORIZONTAL);

    // Rend le bouton de fin de tour et la main.
    buttonRender(renderer, gui->skip_button, gui->hand->m_x, gui->hand->m_y);
    handRender(renderer, gui->hand);

}



// Effectue le rendu du menu pause.
void pauseGUIRender(SDL_Renderer* renderer, PauseGUI* gui) {

    // Assombri l'arrière plan.
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x80);
    SDL_RenderFillRect(renderer, NULL);

    // Rend le texte de pause.
    SDL_Rect label_rect = {
        (gui->w-gui->button_rect.w)/2, gui->button_rect.h,
        gui->button_rect.w, 2*gui->button_rect.h
    };
    SDL_RenderCopy(renderer, gui->pause_label, NULL, &label_rect);

    // Rend le menu de pause.
    SDL_Rect rect;
    switch (gui->state) {
    case PGS_PAUSE:
        for (unsigned int i = 0; i < 3; i++) {
            buttonRender(renderer, gui->buttons[i], gui->m_x, gui->m_y);
        }
        break;
    case PGS_OPTIONS:

        // Rend le label du volume.
        SDL_QueryTexture(gui->volume_label, NULL, NULL, &rect.w, &rect.h);
        float ratio = rect.w/(float)rect.h;
        rect.h = gui->button_rect.h;
        rect.w = gui->button_rect.h*ratio;
        rect.x = (gui->w-rect.w)/2;
        rect.y = gui->volume->rect.y - 1.2*rect.h;
        SDL_RenderCopy(renderer, gui->volume_label, NULL, &rect);

        // Rend le slider du volume.
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &gui->volume->rect);
        SDL_Rect slider_rect = {
            gui->volume->rect.x, gui->volume->rect.y,
            gui->volume->value*gui->volume->rect.w, gui->volume->rect.h
        }; 
        SDL_SetRenderDrawColor(renderer, 0xB0, 0xB0, 0xB0, 0xFF);
        SDL_RenderFillRect(renderer, &slider_rect);

        // Rend le bouton de retour.
        buttonRender(renderer, gui->buttons[3], gui->m_x, gui->m_y);
        break;
    }

}



// Effectue le rendu du menu du deck.
void deckGUIRender(SDL_Renderer* renderer, DeckGUI* gui) {

    // Effectue le rendu du fond.
    SDL_Rect rect = {0, gui->buttons[0]->rect.h, gui->w, gui->h-gui->buttons[0]->rect.h};
    SDL_RenderCopy(renderer, gui->background, NULL, &rect);

    // Effectue le rendu du deck.
    if (gui->state == DK_DECK) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x80);
        SDL_RenderFillRect(renderer, NULL);
    
        // Rend les cartes du deck.
        for (unsigned int i = 0; i < gui->deck_card_entities->size; i++) {
            CardEntity* entity = (CardEntity*)gui->deck_card_entities->data[i];
            SDL_Rect card_rect = {
                entity->rect.x, entity->rect.y - 6*entity->rect.h*gui->offset/5,
                entity->rect.w, entity->rect.h
            };
            SDL_RenderCopy(renderer, entity->carte->texture, NULL, &card_rect);
        }

        // Rend les infobulles sur les cartes.
        if (gui->info_alpha > 0.01f) {
            for (unsigned int i = 0; i < gui->deck_card_entities->size; i++) {
                CardEntity* entity = (CardEntity*)gui->deck_card_entities->data[i];
                if (gui->m_x < entity->rect.x || gui->m_y < entity->rect.y - 6*entity->rect.h*gui->offset/5 || 
                    gui->m_x > entity->rect.x + entity->rect.w || gui->m_y > entity->rect.y + entity->rect.h - 
                    6*entity->rect.h*gui->offset/5) continue;
                renderInfo(renderer, entity, gui->w, gui->h, gui->m_x, gui->m_y, gui->info_alpha);
            } 
        }

    // Effectue le rendu du menu de pull.
    } else {
        
        // Rend l'image de la quantité d'argent.
        SDL_QueryTexture(gui->gold_texture, NULL, NULL, &rect.w, &rect.h);
        float ratio = 0.15/(rect.h/(float)gui->h);
        rect.w *= 1.5*ratio;
        rect.h *= ratio;
        rect.x = (gui->w-rect.w)/2;
        rect.y = gui->h-1.2*rect.h;
        SDL_RenderCopy(renderer, gui->gold_num_texture, NULL, &rect);

        // Rend l'image de l'argent.
        rect.x += 1.05*rect.w;
        rect.y += 0.125*rect.h;
        rect.h *= 0.75;
        rect.w = rect.h;
        SDL_RenderCopy(renderer, gui->gold_texture, NULL, &rect); 

        // Rend les boutons du pull.
        switch (gui->pull_state) {
        case PS_NONE:
            buttonRender(renderer, gui->buttons[2], gui->m_x, gui->m_y);
            break;
        case PS_CHOICE:
            buttonRender(renderer, gui->buttons[3], gui->m_x, gui->m_y);
            buttonRender(renderer, gui->buttons[4], gui->m_x, gui->m_y);
            break;
        default:
            break;
        }
        
        // Rend la carte pull.
        if (gui->entity) SDL_RenderCopy(renderer, gui->entity->carte->texture, NULL, &gui->entity->rect);

        // Rend les infobulles sur les cartes.
        if (gui->entity && gui->info_alpha > 0.01f && gui->m_x >= gui->entity->rect.x 
            && gui->m_y >= gui->entity->rect.y && gui->m_x <= gui->entity->rect.x + gui->entity->rect.w
            && gui->m_y <= gui->entity->rect.y + gui->entity->rect.h) {
            renderInfo(
                renderer, gui->entity, 
                gui->w, gui->h, gui->m_x, gui->m_y, gui->info_alpha
            );
        }

    }

    // Rend les boutons de la barre du haut.
    buttonRender(renderer, gui->buttons[0], gui->m_x, gui->m_y);
    buttonRender(renderer, gui->buttons[1], gui->m_x, gui->m_y);
    
    // Rend des barres noires pour différencier ces boutons.
    SDL_Rect vertical_rect = {gui->buttons[0]->rect.w-1, 0, 2, gui->buttons[0]->rect.h};
    SDL_Rect horizontal_rect = {0, gui->buttons[0]->rect.h-1, gui->buttons[0]->rect.w*2+1, 1};
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &vertical_rect);
    SDL_RenderFillRect(renderer, &horizontal_rect);

}



// Effectue le rendu du menu principal.
void mainMenuRender(SDL_Renderer* renderer, MainMenu* gui) {

    // Rend l'arrière plan.
    SDL_RenderCopy(renderer, gui->background, NULL, &gui->background_rect);

    // Rend le titre du jeu si on est pas dans les credits.
    if (gui->state != MMS_CREDITS) {
        SDL_Rect rect = {
            (gui->w - 1.5*gui->button_rect.w)/2, 
            (0.05 + 0.002*sin(2.0*3.14159265358979323*(SDL_GetTicks()%2000)/2000.0))*gui->w, 
            1.5*gui->button_rect.w, 
            2.0*gui->button_rect.h
        };
        SDL_RenderCopy(renderer, gui->main_label, NULL, &rect);
    }

    // Rend les composants en fonction de l'état du menu.
    float ratio = 1.0f;
    SDL_Rect rect;
    switch (gui->state) {

    // Rend le menu principal.
    case MMS_MAIN:
        for (unsigned int i = 0; i < 4; i++) {
            buttonRender(renderer, gui->buttons[i], gui->m_x, gui->m_y);
        }
        break;

    // Rend les crédits.
    case MMS_CREDITS:
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x80);
        SDL_RenderFillRect(renderer, NULL);

        // Récupère la texture des crédits.
        SDL_QueryTexture(gui->credits, NULL, NULL, &rect.w, &rect.h);
        rect.w *= 0.5;
        rect.h *= 0.5;

        // S'assure qu'elle rentre dans l'écran.
        if (rect.h > 0.9*gui->w) {
            ratio = rect.w/(float)(0.9*gui->w);
            rect.w = rect.w/ratio;
            rect.h = rect.h/ratio;
        }
        if (rect.h > 0.9*gui->h) {
            ratio = rect.h/(float)(0.9*gui->h);
            rect.w = rect.w/ratio;
            rect.h = rect.h/ratio;
        }

        // Rend la texture.
        rect.x = 0.1*gui->w;
        rect.y = 0.05*gui->h;
        SDL_RenderCopy(renderer, gui->credits, NULL, &rect);
        break;

    // Rend les options.
    case MMS_OPTIONS:

        // Rend le label du volume.
        SDL_QueryTexture(gui->volume_label, NULL, NULL, &rect.w, &rect.h);
        float ratio = rect.w/(float)rect.h;
        rect.h = gui->button_rect.h;
        rect.w = gui->button_rect.h*ratio;
        rect.x = (gui->w-rect.w)/2;
        rect.y = gui->volume->rect.y - 1.2*rect.h;
        SDL_RenderCopy(renderer, gui->volume_label, NULL, &rect);

        // Rend le slider du volume.
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &gui->volume->rect);
        SDL_Rect slider_rect = {
            gui->volume->rect.x, gui->volume->rect.y,
            gui->volume->value*gui->volume->rect.w, gui->volume->rect.h
        }; 
        SDL_SetRenderDrawColor(renderer, 0xB0, 0xB0, 0xB0, 0xFF);
        SDL_RenderFillRect(renderer, &slider_rect);
        
        // Rend le bouton de retour.
        buttonRender(renderer, gui->buttons[4], gui->m_x, gui->m_y);
        break;

    // Rend l'interface de lancement de run.
    case MMS_NEW_RUN:
        
        // Rend les boutons.
        for (unsigned int i = 4; i < 10; i++) buttonRender(renderer, gui->buttons[i], gui->m_x, gui->m_y);
        
        // Rend la text box.
        SDL_SetRenderDrawColor(renderer, 0xF0, 0xF0, 0xF0, 0xFF);
        rect.x = gui->buttons[5]->rect.x+gui->buttons[5]->rect.w+4;
        rect.y = gui->buttons[5]->rect.y;
        rect.w = gui->buttons[6]->rect.x-rect.x-4;
        rect.h = gui->buttons[5]->rect.h;
        SDL_RenderFillRect(renderer, &rect);
        rect.y = gui->buttons[7]->rect.y;
        SDL_RenderFillRect(renderer, &rect);
        textBoxRender(renderer, gui->box);

        // Rend le nombre de niveau.
        SDL_QueryTexture(gui->level_number_label, NULL, NULL, &rect.w, &rect.h);
        ratio = rect.w/(float)rect.h;
        rect.h = gui->button_rect.h - 4;
        rect.w = rect.h*ratio;
        rect.y = gui->buttons[5]->rect.y+2;
        rect.x = (gui->buttons[5]->rect.x+gui->buttons[6]->rect.x+gui->buttons[5]->rect.w-rect.w)/2-4;
        SDL_RenderCopy(renderer, gui->level_number_label, NULL, &rect);
        
        // Rend la difficulté.
        SDL_QueryTexture(gui->difficulty_label, NULL, NULL, &rect.w, &rect.h);
        ratio = rect.w/(float)rect.h;
        rect.h = gui->button_rect.h - 4;
        rect.w = rect.h*ratio;
        rect.y = gui->buttons[7]->rect.y+2;
        rect.x = (gui->buttons[7]->rect.x+gui->buttons[8]->rect.x+gui->buttons[7]->rect.w-rect.w)/2-4;
        SDL_RenderCopy(renderer, gui->difficulty_label, NULL, &rect);
        break;
    }

}



// Permet de rendre l'intro.
void introRender(SDL_Renderer* renderer, Intro* intro) {

    // Parcourt les lignes.
    int side = 20;
    float kmax = 1.75*side/2.0;
    int indexes[6] = {0,1,2,2,1,3};
    for (unsigned int i = 0; i < side; i++) {
        
        // Parcourt les colonnes.
        float k = 0;
        for (unsigned int j = 0; j < side; j++) {
            k += 1.0 - 0.25*j/(side-1);

            // Calcule des constantes pour déterminer les sommets du polygone.
            float offset_top = 0.25*intro->w*(intro->h-(intro->render_rect.y+(j/(float)side)*intro->render_rect.h))/(float)intro->h;
            float offset_bottom = 0.25*intro->w*(intro->h-(intro->render_rect.y+((j+1)/(float)side)*intro->render_rect.h))/(float)intro->h;
            float size_top = (intro->w-2.0f*offset_top)/side;
            float size_bottom = (intro->w-2.0f*offset_bottom)/side;
            float texture_top = (k-(1.0 - 0.25*j/(side-1)))/(float)kmax;
            float texture_bottom = k/(float)kmax;

            // Vertices pour le rendu du polygone.
            SDL_Vertex vertices[4] = {
                {{
                    offset_top + i*size_top, 
                    intro->render_rect.y+(j/(float)side)*intro->render_rect.h}, 
                    {0xFF, 0xFF, 0xFF, 0xFF},	
                    {(i/(float)side)*1.0f, (intro->image_rect.y+texture_top*intro->image_rect.h)/(float)intro->tex_h}
                }, {{
                    offset_top + (i+1)*size_top,
                    intro->render_rect.y+(j/(float)side)*intro->render_rect.h}, 
                    {0xFF, 0xFF, 0xFF, 0xFF},
                    {((i+1)/(float)side)*1.0f, (intro->image_rect.y+texture_top*intro->image_rect.h)/(float)intro->tex_h}
                }, {{
                    offset_bottom + i*size_bottom,
                    intro->render_rect.y+((j+1)/(float)side)*intro->render_rect.h}, 
                    {0xFF, 0xFF, 0xFF, 0xFF},	
                    {(i/(float)side)*1.0f, (intro->image_rect.y+texture_bottom*intro->image_rect.h)/(float)intro->tex_h}
                }, {{
                    offset_bottom + (i+1)*size_bottom,
                    intro->render_rect.y+((j+1)/(float)side)*intro->render_rect.h},
                    {0xFF, 0xFF, 0xFF, 0xFF},	
                    {((i+1)/(float)side)*1.0f, (intro->image_rect.y+texture_bottom*intro->image_rect.h)/(float)intro->tex_h}
                },
            };

            // Rend le polygone.
            SDL_RenderGeometry(renderer, intro->texture, vertices, 4, indexes, 6);
        
        }
    }

}





// Effectue le rendu d'une entité dans une map.
void levelEntityRender(SDL_Renderer* renderer, Camera* camera, LevelEntity* entity) {
    
    // Récupère les dimensions d'un tile à l'écran.
    float tile_size;
    cameraGetTileSize(camera, &tile_size);
    
    // Calcule la position du centre de la caméra.
    float center_x = (tile_size)*(camera->x - camera->width/2.0 + 0.5f);
    float center_y = (tile_size)*(camera->y - camera->height/2.0 + 0.5f);

    // Récupère le rect du tile à utiliser.
    SDL_Rect tile_rect;
    tileSetGetRect(entity->tileset, &tile_rect, entity->tile);
  
    // En déduit le rect de l'entité.
    SDL_Rect entity_rect = {
        tile_size*(entity->x-(entity->size-1.0)/2.0) - center_x,
        tile_size*(entity->y-(entity->size-1.0)) - center_y,
        tile_size*entity->size,
        tile_size*tile_rect.h/tile_rect.w*entity->size
    };

    // Vérifie que l'entité est affichable.
    if (entity_rect.x > camera->x+camera->w || entity_rect.y > camera->y+camera->h
        || entity_rect.x+entity_rect.w < camera->x-2*tile_size || entity_rect.y+entity_rect.h < camera->y-2*tile_size) return;
    
    // Rend l'entité.
    SDL_RenderCopy(renderer, entity->tileset->texture, &tile_rect, &entity_rect);

}



// Effectue le rendu d'une série dans une map.
void levelEntitiesRender(SDL_Renderer* renderer, Camera* camera, List* entities) {
    for(unsigned int i = 0; i < entities->size ; i++) {levelEntityRender(renderer, camera, listGet(entities, i));}
}
