#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>





// Structure pour gérer les enrées de texte au clavier.
// font : police pour l'affichage du texte.
// text_texture : texture du texte.
// render_rect, image_rect : rects pour l'affichage du texte.
// rect : taille de la zone d'écriture.
// offset : offset de la texture du texte.
// state : true si le texte doit etre entré.
// cursor_position : position du curseur à l'écran.
// cursor : position du curseur dans le texte.
// text_size : nombre de caracètres dans le texte.
// buffer_size : taille du buffer.
// text : buffer pour le texte.
struct s_TextBox {
    TTF_Font* font;
    SDL_Texture* text_texture;
    SDL_Rect render_rect;
    SDL_Rect image_rect;
    SDL_Rect rect;
    int offset;
    int state;
    int cursor_position;
    unsigned int cursor;
    unsigned int text_size;
    unsigned int buffer_size;
    char* text;
};
typedef struct s_TextBox TextBox;





// Constructeur, destructeur.
TextBox* createTextBox(TTF_Font* font, unsigned int buffer_size, int default_state);
void freeTextBox(TextBox* box);



// Fonctions de manipulation du texte.
void textBoxLeft(TextBox* box);
void textBoxRight(TextBox* box);
void textBoxLeftClick(TextBox* box, int x, int y);
void textBoxProcessEvent(TextBox* box, SDL_Renderer* renderer, SDL_Keycode code);



// Fonction de redimensionnement.
void textBoxResize(TextBox* box, SDL_Rect* rect);
