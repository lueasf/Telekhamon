#include "transition.h"

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>





// Constructeur de Transition.
// x_i, y_i : Départ de la translation.
// x_f, y_f : Arrivée de la translation.
// data : données supplémentaires pour certaines fonctions de transition.
// k, n : étapes courante et max.
// call : fonction de transition qui met à jour les coordonnées (x,y), et qui
// prend en argument la transition parent et des données supplémentaires au besoin.
Transition* createTransition(float x_i, float y_i, float x_f, float y_f, unsigned int n,
    int (*call)(int* x, int* y, struct s_Transition*, void* data)) {
    if (!call) return NULL;

    // Initialise la transition.
    Transition* transition = (Transition*)malloc(sizeof(Transition));
    if (!transition) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION, 
            "Impossible de malloc de la mémoire pour Transition"
        );
        abort();
    }

    // Enregistre ses paramètres et renvoie le résultat.
    transition->call = call;
    transition->x_i = x_i;
    transition->y_i = y_i;
    transition->x_f = x_f;
    transition->y_f = y_f;
    transition->n = n;
    transition->k = 0;
    return transition;

}



// Destructeur de Transition.
void freeTransition(Transition* transition) {
    free(transition);
}





// Fonction de transition de base.
// Transformation linéaire des coordonnées.
// Ne prend pas de data.
int linearTransition(int* x, int* y, Transition* transition, void* data) {

    // Transformation linéaire des coordonnées.
    *x = transition->x_i + transition->k/(float)transition->n*(transition->x_f - transition->x_i);
    *y = transition->y_i + transition->k/(float)transition->n*(transition->y_f - transition->y_i);

    // Passe à la prochaine étape.
    transition->k++;
    return transition->k == transition->n+1;

}



// Fonction de transition de base.
// Transformation sinusoidale des coordonnées.
// Ne prend pas de data.
int sinusTransition(int* x, int* y, Transition* transition, void* data) {

    // Transformation sinusoidale des coordonnées.
    *x = transition->x_i + (transition->x_f-transition->x_i) * 
        sin(1.57079632679 * transition->k/(float)transition->n);
    *y = transition->y_i + (transition->y_f-transition->y_i) * 
        sin(1.57079632679 * transition->k/(float)transition->n);

    // Passe à la prochaine étape.
    transition->k++;
    if (transition->k == transition->n+1) {
        *x = transition->x_f;
        *y = transition->y_f;
    }
    return transition->k == transition->n+1;

}
