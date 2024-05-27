#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>





// Constructeur et destructeur.
TTF_Font* createFont(char* path, int size);
void freeFont(TTF_Font* font);



// Fonction de création de texture depuis un texte.
SDL_Texture* createTextureFromText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color, char* text);
SDL_Texture* createTextureFromTextOutline(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color, 
    SDL_Color* out_color, int out_size, char* text);
SDL_Texture* createTextureFromTextFromated(SDL_Renderer* renderer, TTF_Font* font, const char* text);
SDL_Texture* createTextureFromTextNewLine(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color, 
    char* text, int max_line_size);



// Fonctions de définition du style.
void fontSetBold(TTF_Font* font, int state);
void fontSetItalic(TTF_Font* font, int state);
void fontSetUnderline(TTF_Font* font, int state);
void fontSetStrikethrough(TTF_Font* font, int state);
