#include "mapgen.h"
#include "tergen.h"
#include "../entities/Aetoile.h"



// Renvoie la valeur d'une case de la map.
int getRoomsValueBounded(Rooms* rooms, unsigned int x, unsigned int y, 
    unsigned int x_min, unsigned int y_min) {
    return rooms->map[y+y_min][x+x_min];
}


// Renvoie les lignes/colonnes min/max dans lequels il y a des valeurs. 
// Laise une colonne et une ligne d'écart entre le bort et les pièces.
// Hyp : le contenu de rooms ne touche pas les bords du tableau.
void getRoomsXYMinMax(Rooms* rooms, unsigned int* x_min, unsigned int* x_max,
    unsigned int* y_min, unsigned int* y_max) {
    
    // Met à zéro les valeurs des bornes.
    *x_max = 0;
    *y_max = 0;
    *x_min = rooms->size;
    *y_min = rooms->size;

    // Parcours du tableau pour déterminer les bonnes bornes.
    for (unsigned int i = 0; i < rooms->size; i++) {
        for (unsigned int j = 0; j < rooms->size; j++) {
            if (rooms->map[j][i] == -1) continue;
            if (*x_max < i) *x_max = i;
            if (*y_max < j) *y_max = j;
            if (*x_min > i) *x_min = i;
            if (*y_min > j) *y_min = j;
        }
    }

    // Laisse une ligne et une colonne de chaque coté.
    (*x_max) += 10;
    (*y_max) += 10;
    (*x_min) -= 10;
    (*y_min) -= 11;

}


// Décide des hyperparamètres de la génération de l'étage i et de la position de l'escalier de descente de l'étage précédent
HyperParam* getHyperParameters(int i, int xStairs, int yStairs) {
    HyperParam* hyper = (HyperParam*)malloc(sizeof(HyperParam));
    if (!hyper) {
        printf("Couldn't allocate memory for HyperParam structure\n");
        abort();
    }

    hyper->floor_ = i;

    hyper->dr = 15;
    hyper->n_map = 512; // Largement suffisant même pour le 20° ou 30° étage

    hyper->n_large = 7 + i;
    hyper->n_small = 20 + 3*i;
    hyper->n_lhidden = 5 + (int)0.4*i;
    hyper->n_shidden = 10 + (int)0.5*i;

    hyper->n_proxi = 7;

    hyper->max_r = 25. + 2*i;
    hyper->min_theta = PI/2.;

    hyper->corridor_width = 2;
    hyper->tile_gap = 1;

    hyper->xStairs = xStairs;
    hyper->yStairs = yStairs;

    hyper->nb_ennemis = hyper->n_large/2 + (int).4*i;

    return hyper;
}


// Fonction de coloriage d'une map à partir d'un Rooms.
// xStairs, yStairs = coordonnées de l'escalier de descente de l'étage précédent.
// tilesets : une liste de l tilesets, un pour chaque couche de la map.
// entity_height : nombre de couches sous les entités.
Map* generateMap(int floor, int xStairs, int yStairs, TileSet** tilesets, LevelEntity* momie, TileSet* ennemi_map_tileset) {
    // Génère les rooms, jusqu'à obtenir une map connexe.
    HyperParam* hyper = getHyperParameters(floor, xStairs, yStairs);
    int nb_ennemis = hyper->nb_ennemis;
    List* entities = createList(nb_ennemis+1);
    Rooms* rooms = generateRooms(hyper, momie, entities, ennemi_map_tileset);
    int connexe = checkConnexite(rooms);
    while (!connexe) {
        freeRooms(rooms);
        listClear(entities);
        rooms = generateRooms(hyper, momie, entities, ennemi_map_tileset);
        connexe = checkConnexite(rooms);
    }
    free(hyper);

    // Récupère les bornes de rooms.
    unsigned int x_min;
    unsigned int x_max;
    unsigned int y_min;
    unsigned int y_max;
    getRoomsXYMinMax(rooms, &x_min, &x_max, &y_min, &y_max);
    
    // Crée une map vide.
    unsigned int w = x_max-x_min+1, h = y_max-y_min+1;
    Map* map = createMap(tilesets, 3, 2, w,h);
    
    // Rajoute la liste des entités et les xyStairs
    freeList(map->entities);
    map->entities = entities;
    map->xStairs = rooms->xDownStairs;
    map->yStairs = rooms->yDownStairs;

    // Parcourt la map pour faire son coloriage.
    for (unsigned int i = 0; i < w; i++) {
        for (unsigned int j = 0; j < h; j++) {
            int tileValue = getRoomsValueBounded(rooms, i,j, x_min, y_min);

            if (i == 0 && j == 0) {mapSet(map, 0, i, j, 9);} // Remplir les contours de la map
            else if (i == 0 && j == h - 1) {mapSet(map, 0, i, j, 21);}
            else if (i == w - 1 && j == 0) {mapSet(map, 0, i, j, 11);}
            else if (i == w - 1 && j == h - 1) {mapSet(map, 0, i, j, 23);}
            else if (i == 0) {mapSet(map, 0, i, j, 15);}
            else if (j == 0) {mapSet(map, 0, i, j, 10);}
            else if (i == w - 1) {mapSet(map, 0, i, j, 17);}
            else if (j == h - 1) {mapSet(map, 0, i, j, 22);}

            else if (tileValue == 2942) { // Escalier de spawn
                // Réupère les tiles alentours.
                int around[9] = {
                    getRoomsValueBounded(rooms, i,  j-1, x_min, y_min) == 2942, // N  : 1 -> 0
                    getRoomsValueBounded(rooms, i+1,j,   x_min, y_min) == 2942, // W  : 3 -> 1
                    getRoomsValueBounded(rooms, i-1,j,   x_min, y_min) == 2942, // E  : 4 -> 2
                    getRoomsValueBounded(rooms, i  ,j+1, x_min, y_min) == 2942, // S  : 6 -> 3
                    getRoomsValueBounded(rooms, i+1,j+1, x_min, y_min) == 2942, // SW : 5 -> 4
                    getRoomsValueBounded(rooms, i-1,j+1, x_min, y_min) == 2942, // SE : 7 -> 5
                    getRoomsValueBounded(rooms, i+1,j-1, x_min, y_min) == 2942, // NW :  -> 6
                    getRoomsValueBounded(rooms, i-1,j-1, x_min, y_min) == 2942, // NE :  -> 7
                    getRoomsValueBounded(rooms, i,  j+2, x_min, y_min) == 2942  // SS : 8
                };

                if (around[2] && !around[7]) {mapSet(map, 0, i, j, 31);}
                else if (around[1] && !around[6]) {mapSet(map, 0, i, j, 30);}
                else if (around[7] && around[5]) {mapSet(map, 0, i, j, 33);}
                else if (around[6] && around[4]) {mapSet(map, 0, i, j, 32);}
                else if (around[2] && !around[5]) {mapSet(map, 0, i, j, 35);}
                else {mapSet(map, 0, i, j, 34);}
            }
            else if (tileValue == 2578) { // Escalier de descente
                // Réupère les tiles alentours.
                int around[9] = {
                    getRoomsValueBounded(rooms, i,  j-1, x_min, y_min) == 2578, // N  : 1 -> 0
                    getRoomsValueBounded(rooms, i+1,j,   x_min, y_min) == 2578, // W  : 3 -> 1
                    getRoomsValueBounded(rooms, i-1,j,   x_min, y_min) == 2578, // E  : 4 -> 2
                    getRoomsValueBounded(rooms, i  ,j+1, x_min, y_min) == 2578, // S  : 6 -> 3
                    getRoomsValueBounded(rooms, i+1,j+1, x_min, y_min) == 2578, // SW : 5 -> 4
                    getRoomsValueBounded(rooms, i-1,j+1, x_min, y_min) == 2578, // SE : 7 -> 5
                    getRoomsValueBounded(rooms, i+1,j-1, x_min, y_min) == 2578, // NW :  -> 6
                    getRoomsValueBounded(rooms, i-1,j-1, x_min, y_min) == 2578, // NE :  -> 7
                    getRoomsValueBounded(rooms, i,  j+2, x_min, y_min) == 2578  // SS : 8
                };

                if (around[2] && !around[7]) {mapSet(map, 0, i, j, 37);}
                else if (around[1] && !around[6]) {mapSet(map, 0, i, j, 36);}
                else if (around[7] && around[5]) {mapSet(map, 0, i, j, 39);}
                else if (around[6] && around[4]) {mapSet(map, 0, i, j, 38);}
                else if (around[2] && !around[5]) {mapSet(map, 0, i, j, 41);}
                else {mapSet(map, 0, i, j, 40);}
            }
            else if (tileValue != -1) {mapSet(map, 0, i, j, 16);} // Espace encombré
            else {  // Murs

                // Réupère les tiles alentours.
                int around[9] = {
                    getRoomsValueBounded(rooms, i,  j-1, x_min, y_min) == -1, // N  : 1 -> 0
                    getRoomsValueBounded(rooms, i+1,j,   x_min, y_min) == -1, // W  : 3 -> 1
                    getRoomsValueBounded(rooms, i-1,j,   x_min, y_min) == -1, // E  : 4 -> 2
                    getRoomsValueBounded(rooms, i  ,j+1, x_min, y_min) == -1, // S  : 6 -> 3
                    getRoomsValueBounded(rooms, i+1,j+1, x_min, y_min) == -1, // SW : 5 -> 4
                    getRoomsValueBounded(rooms, i-1,j+1, x_min, y_min) == -1, // SE : 7 -> 5
                    getRoomsValueBounded(rooms, i+1,j-1, x_min, y_min) == -1, // NW :  -> 6
                    getRoomsValueBounded(rooms, i-1,j-1, x_min, y_min) == -1, // NE :  -> 7
                    getRoomsValueBounded(rooms, i,  j+2, x_min, y_min) == -1  // SS : 8
                };

                // Toute un série de ifs dégeulasses pour traiter au cas par cas quel tile afficher.
                // C'est moche, mais ça marche bien.
                if (around[0] && !around[1] && !around[2] && !around[3]) {mapSet(map, 0, i, j, 18);}
                else if (!around[0] && !around[1] && !around[2] && around[3]) {mapSet(map, 0, i, j, 19);}
                else if (around[0] && !around[1] && !around[2] && around[3]) {mapSet(map, 0, i, j, 20);}
                else if (!around[0] && around[1] && around[2] && around[3]) {mapSet(map, 0, i, j, 22);}
                else if (around[1] && around[2] && !around[3]) {mapSet(map, 0, i, j, 10);}
                else if (around[0] && around[1] && !around[2] && around[3]) {mapSet(map, 0, i, j, 17);}
                else if (around[0] && !around[1] && around[2] && around[3]) {mapSet(map, 0, i, j, 15);}
                else if (!around[1] && around[2] && !around[3]) {mapSet(map, 0, i, j, 12);}
                else if (around[1] && !around[2] && !around[3]) {mapSet(map, 0, i, j, 14);}
                else if (around[0] && around[1] && around[2] && around[3] && !around[6]) {mapSet(map, 0, i, j, 21);}
                else if (around[0] && around[1] && around[2] && around[3] && !around[7]) {mapSet(map, 0, i, j, 23);}
                else if (!around[0] && around[1] && !around[2] && around[3]) {mapSet(map, 0, i, j, 24);}
                else if (!around[0] && !around[1] && around[2] && around[3]) {mapSet(map, 0, i, j, 25);}
                else if (!around[4] && around[1] && around[2] && around[3]) {mapSet(map, 0, i, j, 9);}
                else if (!around[5] && around[1] && around[2] && around[3]) {mapSet(map, 0, i, j, 11);}
                else if (!around[0] && around[1] && !around[2] && around[3] && !around[8]) {mapSet(map, 0, i, j, 1);}
                else if (!around[0] && !around[1] && around[2] && around[3] && !around[8]) {mapSet(map, 0, i, j, 2);}
                else {
                    if (rand()%ONE_OBJECT_OUT_OF == 0) {
                        mapSet(map, 0, i, j, 42 + 1+rand()%OBJECT_TEXTURE_COUNT);
                    } else {
                        mapSet(map, 0, i, j, 42);
                    }
                }

            }
        }
    }

    // Parcourt les tiles ajoutés précédements. 
    for (unsigned int i = 0; i < w; i++) {
        for (unsigned int j = 1; j < h; j++) {
            unsigned int id = mapGet(map, 0, i, j);
            int upper = mapGet(map, 0, i, j-1) != 16;
            
            // Ajoute d'autres tiles pour les murs de 2 de haut.
            switch (id) {
            case 9:
                mapSet(map, 0, i, j-1, 3);
                break;
            case 10:
                mapSet(map, 2, i, j-1, 4);
                break;
            case 11:
                mapSet(map, 0, i, j-1, 5);
                break;
            case 12:
                if (upper) mapSet(map, 2, i, j-1, 6);
                else mapSet(map, 2, i, j-1, 13);
                break;
            case 14:
                if (upper) mapSet(map, 2, i, j-1, 8);
                else mapSet(map, 2, i, j-1, 7);
                break;
            };

        }
    }
   

    // Création des positions initiale des ennemis
    for(int i=0;i<nb_ennemis;i++){
        int id=rand()%6+1;
        LevelEntity* ennemi = createLevelEntity(ennemi_map_tileset,id,momie->size,0,0);
        int not_valid=1;
        while(not_valid){
            int starting_position_x = rand()%map->layers[0]->w;
            int starting_position_y = rand()%map->layers[0]->h;
            if (mapGet(map, 0, starting_position_x, starting_position_y) != 16) continue;

            ennemi->x = starting_position_x;
            ennemi->y = starting_position_y;
            
            for(int j=0;j<entities->size;j++){
                LevelEntity* entity=(LevelEntity*)entities->data[j];
                if((manhattanDistance(ennemi->x,ennemi->y,entity->x,entity->x)) > 12.){
                    not_valid=0;
                    break;
                }

            }
        }

        listAdd(entities,ennemi);
        
    }


    // Renvoie la map coloriée.
    freeRooms(rooms);
    
    return map;

}
