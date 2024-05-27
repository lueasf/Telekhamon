#pragma once





// Structure pour modéliser une transition entre 2 états d'un objet.
// Sert principalement à faire des translations d'objets.
// x_i, y_i : Départ de la translation.
// x_f, y_f : Arrivée de la translation.
// data : données supplémentaires pour certaines fonctions de transition.
// k, n : étapes courante et max.
// call : fonction de transition qui met à jour les coordonnées (x,y), et qui
// prend en argument la transition parent et des données supplémentaires au besoin.
struct s_Transition {
    float x_i;
    float y_i;
    float x_f;
    float y_f;
    void* data;
    unsigned int k;
    unsigned int n;
    int (*call)(int*, int*, struct s_Transition*, void* data);
};
typedef struct s_Transition Transition;





// Constructeur et destructeur.
Transition* createTransition(float x_i, float y_i, float x_f, float y_f, unsigned int n,
    int (*call)(int* x, int* y, Transition*, void* data));
void freeTransition(Transition* transition);



// Fonctions de transitions de base.
int linearTransition(int* x, int* y, Transition* transition, void* data);
int sinusTransition(int* x, int* y, Transition* transition, void* data);
