#include "Aetoile.h"

#include "../utils/priority_queue.h"
#include "../utils/list.h"

#include <stdlib.h>
#include <math.h>





// Constructeur de Point.
Point* createPoint(int x, int y) {
    Point* point = (Point*)malloc(sizeof(Point));
    point->x = x;
    point->y = y;
    return point;
}



// Destructeur de Point.
void freePoint(Point* point) {
    free(point);
}





A_noeud* createnode(int x, int y, float g, float h, A_noeud* parent){
    A_noeud* noeud = (A_noeud*)malloc(sizeof(A_noeud));
    noeud->x = x;
    noeud->y = y;
    noeud->g = g;
    noeud->h = h;
    noeud->f = h + g;
    noeud->parent = parent;
    return noeud;
}



void destroynode(A_noeud* noeud){
    free(noeud);    
}





// fonction de calcul de l'heuristique basé sur la distance de manhattan
// pour des déplacements horizontaux et verticaux (taxi-distance wsh)
float manhattanDistance(int x1, int y1, int x2, int y2){
    return (float)(fabs((float)x1 - (float)x2) + fabs(((float)y1 - (float)y2)));
}





// Attention, le paramètre width et height ne sont pas obligatoire dans findPath
// en effet, la map n'étant pas rectangulaire, les paramètres sont difficile à calculer 
// CONCLUSION : On peut s'en passer
Stack* findPath(LevelEntity* entity, LevelEntity* player, Map* map) { 
    if (!entity || !player) return NULL;
    int width  = map->layers[0]->w;
    int height = map->layers[0]->h;

    PriorityQueue* ouvert = createPriorityQueue(); // liste ouverte (pas visité)
    List* ferme = createList(64); // liste fermée (visité)

    // noeud de départ
    A_noeud* depart = createnode(
        (int)round(entity->x), (int)round(entity->y), 0.0f,
        manhattanDistance((int)round(entity->x), (int)round(entity->y), (int)player->x, (int)player->y),
        NULL
    );
    priorityQueueInsert(ouvert, depart, depart->f);

    //A* boucle principale de findPath
    while(priorityQueueSize(ouvert) > 0) { // tant que la liste des cases non visitées n'est pas nulle ...

        //1. trouver le noeud avec le coût f le plus bas:
        A_noeud* current = (A_noeud*)priorityQueuePop(ouvert); 
        // au début, current est le noeud de depart, puis on change etc

        // vérifier si le current contient le joueur:
        // dans ce cas on a trouvé le chemin le plus court.
        if (current->x == (int)player->x && current->y == (int)player->y ){ // plutot mettre si la cible est a une case d'écart
            
            // SI ON TROUVE le joueur on reconstruit le chemin avec les parents
            Stack* chemin = createStack();
            while (current != NULL) {
                stackPush(chemin, createPoint(current->x, current->y));
                current = current->parent;
            }

            // Supprime les données de la recherche.
            for (unsigned int i = 0; i < listSize(ferme); i++) {
                destroynode((A_noeud*)listGet(ferme, i));
            }
            PriorityNode* node = ouvert->first;
            while (node != NULL) {
                destroynode(node->value);
                node = node->next;
            }
            freePriorityQueue(ouvert);
            freeList(ferme);

            // Renvoie le chemin trouvée.
            return chemin;

        }

        //explorerles voisins:
        for (int dx = -1; dx <= 1; dx++){
            for (int dy = -1; dy <= 1; dy++){
                if (dx == 0 && dy == 0){
                    continue; // case current
                }
                int vx = current->x + dx; // abscisse du voisin
                int vy = current->y + dy; // ordonnée du voisin

                if (vx >= 0 && vx < width && vy >= 0 && vy < height && mapGet(map, 0, vx, vy) == 16){ // PB : la map est pas carré ou rectangulaire.
                    // si l'abscisse de l'ordonnée du voisin est dans la map et ne casse pas de mur.
                    int index_ferme = -1;
                    for (int i=0; i< listSize(ferme); i++){
                        A_noeud* node = (A_noeud*)listGet(ferme, i);
                        if (node->x == vx && node->y == vy){
                            index_ferme = i; // on a déjà visité cette case
                            break;
                        }
                    }
                    if (index_ferme != -1){
                        continue; // on passe à la case suivante car celle-ci a déjà été visitée.
                    }

                    PriorityNode* p_node = ouvert->first;
                    while (p_node != NULL) {
                        A_noeud* node = (A_noeud*)p_node->value;
                        if (node->x == vx && node->y == vy) break;
                        p_node = p_node->next;
                    }

                    if (p_node != NULL) {
                        A_noeud* node = (A_noeud*)p_node->value;
                        if (node->g <= current->f+1) continue;
                        node->g = current->f+1;
                    }

                    if (p_node == NULL) {
                        A_noeud* node = createnode(
                            vx, vy, current->f+1, 
                            manhattanDistance(vx, vy, (int)player->x, (int)player->y), 
                            current
                        );
                        priorityQueueInsert(ouvert, node, 0.0f);
                        p_node = ouvert->first;
                    }

                    p_node->key = ((A_noeud*)p_node->value)->g + manhattanDistance(vx, vy, (int)player->x, (int)player->y);
                    ((A_noeud*)p_node->value)->parent = current;

                }
            }
        } 
        listAdd(ferme, current); // on met le noeud courrant dans la liste ferme.

    }
    return NULL;

}
