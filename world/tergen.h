# pragma once

#include "../entities/level_entity.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include "pile.h"




// Structure d'une room
// type : type de room : "large", "small", "vertical" corridor or "horizontal" corridor
// nb : indice de la room (dans la liste triée par rayons, généralement)
// w : largeur en tiles
// h : hauteur en tiles
// r : rayon du centre de la room
// theta : angle du centre de la room
// x : position horizontale du coin bas gauche de la room ;
//     initialisé à 0 et modifiée seulement lorsque la room est fixée.
// y : position verticale du coin bas gauche de la room ;
//     initialisé à 0 et modifiée seulement lorsque la room est fixée.
struct s_Room {
    char* type;
    int nb;
    unsigned int w;
    unsigned int h;
    float r;
    float theta;
    int x;
    int y;
};
typedef struct s_Room Room;



// Structure d'une liste de proximité :
// room : room de laquelle on établit la liste de proximité
// proxi : poiteur vers la liste des rooms les plus proches de room
struct s_ProxiList {
    Room* room;
    Room** proxi;
};
typedef struct s_ProxiList ProxiList;



// Conteneur de la map générée.
// size : taille de la map.
// map : tableau de taille size*size.
// MEMO map :
// -1 = vide
// 0, 1, 2, ..., (N_LARGE+N_SMALL+N_LHIDDEN+N_SHIDDEN) = room
// 2842 = chemin
// 2942 = escaliers d'arrivée
// 2578 = escaliers de sortie
struct s_Rooms {
    int size;
    int xDownStairs;
    int yDownStairs;
    int** map;
}; 
typedef struct s_Rooms Rooms;


// floor_ = Etage à générer = Nombre d'étage que la momie a descendu pour arriver à cet étage.
// dr = De combien espacer chaque room à chaque itération lors de la phase de dispersion.
// n_map = Taille de la map (x et y max (resp. min) en positif (resp. négatif)).
// n_large, n_small, n_lhidden, n_shidden = Nombre de rooms large ou small, visible ou non à générer.
// n_proxi = Nombre de rooms les plus proche regardées lors de la décision des chemins.
// max_r, min_theta = Paramètres de la décision des chemins. R en tiles (chemin de longueur maximum), THETA en rad (chemin le plus proche en angle d'un autre chemin).
// corridor_width = Largeur en tiles des chemins.
// tile_gap = Nombre de tiles à laisser au minimum entre les chemins et le bord des rooms.
// xStairs, yStairs = Coordonnées de l'escalier de descente de l'étage précédent.
// nb_ennemis = Nombre de silhouettes de groupes d'ennemis sur la map.
struct s_HyperParam {
    int floor_;

    float dr;

    int n_map;

    int n_large;
    int n_small;
    int n_lhidden;
    int n_shidden;

    int n_proxi;
    
    float max_r;
    float min_theta;

    int corridor_width;
    int tile_gap;

    int xStairs;
    int yStairs;

    int nb_ennemis;
};
typedef struct s_HyperParam HyperParam;


// Initialise une room avec toutes ses caractéristiques nulles
Room* initRoom(void);

// Génération de room
Room* createRoom(char* type, unsigned int minw, unsigned int maxw, unsigned int minh, unsigned int maxh);

// Génération d'une liste de rooms
Room** createRoomList(unsigned int n, char* type, unsigned int minw, unsigned int maxw, unsigned int minh, unsigned int maxh);

// Constructeur de Rooms.
Rooms* createRooms(int size);



// Destructeur de Room.
void freeRoom(Room* room);

// Destructeur d'une liste de Room.
void freeRoomList(Room** rooms, unsigned int n);

// Destructeur de Rooms.
void freeRooms(Rooms* rooms);



// Trie une liste de rooms par rayon croissant
void sortRooms(Room** rlist, int n_room);

// Vérifie si deux rooms sont superposées
int collide(Room* r1, Room* r2);

// Vérifie, dans une liste de rooms rlist, si la room n°i
// rentre en collision avec l'une des précédentes.
// Si il n'y a pas de collision, retourne -1.
// S'il y en a une, retourne le numéro de la room touchée.
int collides(Room** rlist, int i);

// Algorithme pour rendre entières les coordonnées de la room
// n°i sans qu'elle rentre en collision avec la room n°lastc
// (et donc normalement aucune des autres).
void intifyRoom(Room** rlist, int i, int lastc);

// Calcule la distance entre le centre de deux rooms
float dist(Room* room1, Room* room2);

// Calcule l'angle en radians entre le vecteur (1, 0),
// et le vecteur centreroom1--->centreroom2.
// Le résultat est compris entre 0 et 2*pi.
float angle(Room* room1, Room* room2);

// Crée la liste de proximité de la room n°i de la liste de rooms rlist.
// La liste de proximité d'une room r est la liste des coordonnées
// polaires (où le centre des coordonnées polaires n'est pas 0 mais le centre de r)
// des n rooms les plus proche de cette room r
ProxiList* createProximityList(Room** rlist, int n_room, int i, int n);

// Crée la liste des listes de proximité d'une liste de rooms.
// La liste de proximité d'une room r est la liste des coordonnées
// polaires (où le centre des coordonnées polaires n'est pas 0 mais le centre de r)
// des n rooms les plus proche de cette room r
ProxiList** createProximityLists(Room** rlist, int n_room, int n);



// Destructeur de ProxiList.
void freeProximityList(ProxiList* list, unsigned int k);

// Destructeur d'une liste de ProxiList
void freeProximityLists(ProxiList** lists, unsigned int n, unsigned int k);



// Calcule le plus petit écart entre le theta de la room n°i et
// celui de n'importe quelle room la précédant
float closestTheta(Room** proxi, int i);

// Retourne la minimum entre les entiers x et y
int min(int x, int y);

// Retourne la maximum entre les entiers x et y
int max(int x, int y);

// Enlève les doublons d'une liste de liens
int* removeDuplicates(int* linksList);

// Décide de quelles liaisons maintenir à partir de la liste de proximité proxiList et des hyperparamètres
// Renvoie une liste d'entiers, qui est en réalité la liste des couples des indices des rooms constituant chaque liaison.
int* decideOfLinks(ProxiList** pLists, int n_room, int n_proxi, float max_r, float min_theta);

// Checks whether element elem is already present at index i
// or before in the list list.
int notIn(int* list, int elem, int i);

// rlist must be the whole list of rooms.
// Prints the tile (i, j) on the map under the condition that
// the tile in (i, j) is free (=-1) or occupied by a small room,
// in which case it appends its number to the smallShow list, at position sswi.
// Returns the new sswi, which is incremented if used.
int conditionPrint(int** map, int n_map, int i, int j, Room** rlist, int* smallShow, int sswi);

// rlist must be the whole list of rooms.
// Creates one link (potentially two, see gen.txt) between room1 and room2, and prints it on the map.
// tmin and cor_w are hyperparameters, passed respectively for choosing the number of tiles that should
// be left free of links tiles, on the edge of rooms (see draft.png); and the corridor's width.
// Please note that both room's w and h must respect w <= 2*(cor_w + 2*tmin) - 1 >= h.
// Returns the new sswi
int printLinkToMap(int** map, int n_map, Room* room1, Room* room2, int tmin, int cor_w, Room** rlist, int* smallShow, int sswi);

// Prints all links from a linksList to a map
// Returns the list of small rooms to be displayed
int* printAllLinksToMap(int** map, int n_map, Room** rlist, int* linksList, int tmin, int cor_w, int n_small);


void getRoomsXYMinMaxWish(int** map, int size, unsigned int* x_min, unsigned int* x_max,
    unsigned int* y_min, unsigned int* y_max);

// Fonction de génération d'une map.
// Renvoie un Rooms dont es éléments sont des ids de room.
Rooms* generateRooms(HyperParam* hyper, LevelEntity* momie, List* entities, TileSet* ennemi_map_tileset);

int checkConnexite(Rooms* rooms);