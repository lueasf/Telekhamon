#include "intro.h"

#include <stdlib.h>
#include <stdio.h>

#include "../graphics/font.h"





#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))





// Constructeur de Intro.
Intro* createIntro(SDL_Renderer* renderer, TTF_Font* font, unsigned int w, unsigned int h) {

    // Malloc l'intro.
    Intro* intro = (Intro*)malloc(sizeof(Intro));
    if (!intro) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour createIntro");
        abort();
    }

    // Crée la texture de l'intro.
    SDL_Color color = {0xFF, 0xE8, 0x1F, 0xFF};
    intro->texture = createTextureFromTextNewLine(renderer, font, &color, 
        "A long time ago in a galaxy far far away...\n"
        "It is a period of civil wars in the galaxy. A brave alliance of underground freedom fighters has challenged the tyranny and oppression of the awesome GALACTIC EMPIRE.\n"
        "Striking from a fortress hidden among the billion stars of the galaxy, rebel spaceships have won their first victory in a battle with the powerful Imperial Starfleet. The EMPIRE fears that another defeat could bring a thousand more solar systems into the rebellion, and Imperial control over the galaxy would be lost forever.\n"
        "To crush the rebellion once and for all, the EMPIRE is constructing a sinister new battle station.  Powerful enough to destroy an entire planet, its completion spells certain doom for the champions of freedom.\n"
        "But this is not the story we will be telling here...\n"
        "However, in a galaxy not so far away in fact, a few millions years ago...\n"
        "It all begun when a dyslexic, mute and blind alien arrived on a planet that will later be know as earth, ignorant of his own conditions, he said the following\n"
        "\"Let there be light\"\n"
        "But since he was dyslexic he miss typed it and the TTR (text to reality) machine thus created life, and that is how cats appeared on planet earth.\n"
        "The alien not knowing how to react decided to leave the planet to look for a hole where to hide himself in shame until his final day so let's forget about him.\n"
        "The cat then started to feel alone, and that is why he used the TTR machine left by the alien to create more cats, leading to the first civilisation ever seen on earth.\n"
        "However, cats cannot bear to work, so the first cat had the awesome idea to create autonomous flesh golems with an efficient design to be able to serve them, however the TTR auto corrected the cat's prompt and is how they ended with an army of \"Rebellious flesh golems\" that will later be known as humans.\n"
        "Those golems were able of a feat close to self replication called reproduction, and rapidly dominated the planet, however the cats couldn't give up their paradise, so they decided to manipulate the golems in the shadows, which wasn't too hard since they were not that bright...\n"
        "This is how the second civilisation ever appeared on earth.\n"
        "Trying to find their creator, the cats made humans build huge WI-FI relays that are now known as pyramids, to shoot the 10 hours nyan cat video through the whole universe, but this is yet another story let's stay focused...\n"
        "Such a huge project needed a good leader, and this is where the most famous tyrant of ancient Egypt, which is also the main character of this story appears, the infamous Telekhamon.\n"
        "He believed that he heard the voices of gods, which were in fact the cats' voices, and used the divine revelations to convert every single alive (or dead) human into a cult member.\n"
        "He didn't live a long life since as soon as the WI-FI relays were done, he was no longer useful to the cats, so they decided to trap him in the biggest relay there were so that he would never escape, after rolling him in toilet paper because it sounded like fun of course.\n"
        "We are now a few million years later, the story that was just told is now nothing but an ancient legend, however, Telekhamon is indeed real and still wander through his WI-FI relay getting rid of both treppassers and residents in his thrist for revenge, and we shall oversee that revenge.\n"
        "Why you ask ? because that looks fun duh !\n"
        "Telekhamon never gonna give you up\n"
        "He never gonna let you down\n"
        "Never gonna run around and desert you\n"
        "Never gonna make you cry\n"
        "Never gonna say goodbye\n"
        "Never gonna tell a lie and hurt you\n",
        80
    );

    // Met à zéro les paramètres et la taille de l'intro.
    intro->offset = 0;
    introSetStreenSize(intro, w,h);

    // Renvoie l'intro initialisée.
    return intro;

}



// Destructeur de Intro.
void freeIntro(Intro* intro) {
    SDL_DestroyTexture(intro->texture);
    free(intro);
}





// Fonction d'update.
void introUpdate(Intro* intro) {
    intro->offset += 1;
    introUpdateRects(intro);
}



// Renvoie true si l'intro est finie.
int introIsDone(Intro* intro) {
    return intro->image_rect.y >= intro->tex_h;
}





// Fonction de redimensionnement.
void introSetStreenSize(Intro* intro, unsigned int w, unsigned int h) {
    intro->w = w;
    intro->h = h;

    // Recalcule la hauteur du rect du champ de vision sur l'image.
    SDL_QueryTexture(intro->texture, NULL, NULL, &intro->tex_w, &intro->tex_h);
    intro->img_h = intro->tex_w*h/w;

    // Met à jour les rects.
    introUpdateRects(intro);

}



// Recalcule les rects pour l'affichage.
void introUpdateRects(Intro* intro) {

    // Calcule le rect de l'image.
    intro->image_rect.x = 0;
    intro->image_rect.y = max(intro->offset - intro->img_h, 0);
    intro->image_rect.w = intro->tex_w;
    intro->image_rect.h = min(intro->offset - intro->img_h + intro->img_h, intro->img_h);
    if (intro->image_rect.y+intro->image_rect.h > intro->tex_h) {
        intro->image_rect.h = max(intro->tex_h-intro->image_rect.y, 0);
    }

    // Calcule le rect pour le rendu.
    intro->render_rect.x = 0;
    intro->render_rect.y = -min((intro->offset - intro->img_h)/(float)intro->img_h*intro->h, 0);
    intro->render_rect.w = intro->w;
    intro->render_rect.h = intro->h*(float)intro->image_rect.h/intro->img_h;

}
