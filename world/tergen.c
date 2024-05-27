
#include "tergen.h"
#include "../entities/Aetoile.h"



// Initialise une room avec toutes ses caractéristiques nulles
Room* initRoom(void) {
    Room* room = (Room*)malloc(sizeof(Room));

    room->w = 0;
    room->h = 0;

    room->r = 0;
    room->theta = 0;

    room->type = NULL;
    room->nb = -1;

    room->x = 0;
    room->y = 0;
    
    return room;
}


// Génère une room de taille aléatoire entre les paramètres donnés
Room* createRoom(char* type, unsigned int minw, unsigned int maxw, unsigned int minh, unsigned int maxh) {
    if (maxw < minw || maxh < minh) {return 0;};

    Room* room = (Room*)malloc(sizeof(Room));

    // Générer les valeurs random
    room->w = rand()%(maxw-minw+1) + minw;
    room->h = rand()%(maxh-minh+1) + minh;

    float pi = 3.141592654;
    room->r = rand()/(float)RAND_MAX; // €[0, 1]
    room->theta = rand() * 2.*pi / (float)RAND_MAX; // €[0, 2pi]

    // Remplir les autres attributs    
    room->type = type;

    room->x = 0;
    room->y = 0;
    
    return room;
}


// Génère une liste de rooms de taille aléatoire entre les paramètres donnés
Room** createRoomList(unsigned int n, char* type, unsigned int minw, unsigned int maxw, unsigned int minh, unsigned int maxh) {
    Room** rlist = (Room**)malloc(n*sizeof(Room*));
    
    Room* room;
    for (int i=0; i<n; i++) {
        room = createRoom(type, minw, maxw, minh, maxh);
        rlist[i] = room;
    }

    return rlist;
}


// Constructeur de Rooms.
Rooms* createRooms(int size) {
    assert(size > 0);
    
    Rooms* rooms = (Rooms*)malloc(sizeof(Rooms));
    rooms->size = size;
    rooms->map = (int**)malloc(size*sizeof(int*));
    for (unsigned int i = 0; i < size; i++) {
        rooms->map[i] = (int*)malloc(size*sizeof(int));
        for (unsigned int j = 0; j < size; j++) {
            rooms->map[i][j] = -1;
        }
    }

    return rooms;
}



// Destructeur de Room.
void freeRoom(Room* room) {
    free(room);
}


// Destructeur d'une liste de Room.
void freeRoomList(Room** rooms, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        free(rooms[i]);
    }
    free(rooms);
}


// Destructeur de Rooms.
void freeRooms(Rooms* rooms) {
    if (!rooms) return;
    for (unsigned int i = 0; i < rooms->size; i++) {
        free(rooms->map[i]);
    }
    free(rooms->map);
    free(rooms);
}



// Trie une liste de rooms par rayon croissant (tri à bulle descendant)
void sortRooms(Room** rlist, int n_room) {
    Room* r1; Room* r2;
    for (int i = 0; i < n_room; i++) {
        for (int j = i - 1; j >= 0; j--) {
            if (rlist[j+1]->r < rlist[j]->r) {
                // Inverser les deux pointers
                r1 = rlist[j]; r2 = rlist[j+1];
                rlist[j] = r2; rlist[j+1] = r1;
            }
        }
    }
}


// Vérifie si deux rooms sont superposées
// Sachant que room1 est fixée, et que room2 ne l'est pas encore.
int collide(Room* room1, Room* room2) {
    float r2 = room2->r;
    float t2 = room2->theta;
    
    float w1 = (float)room1->w; float w2 = (float)room2->w;
    float h1 = (float)room1->h; float h2 = (float)room2->h;

    float x1 = (float)room1->x; float x2 = r2*cos(t2);
    float y1 = (float)room1->y; float y2 = r2*sin(t2);

    int collision = !(
        (x2 + w2/2. < x1) ||
        (y2 + h2/2. < y1) ||
        (x1 + w1 < x2 - w2/2.) ||
        (y1 + h1 < y2 - h2/2.)
    );

    return collision;
}


// Vérifie, dans une liste de rooms rlist, si la room n°i
// rentre en collision avec l'une des précédentes.
// Si il n'y a pas de collision, retourne -1.
// S'il y en a une, retourne le numéro de la room touchée.
int collides(Room** rlist, int i) {
    Room* roomi = rlist[i];
    int res = -1;

    for (int j = 0; j < i; j++) {
        if (collide(rlist[j], roomi)) {
            res = j;
        }
    }

    return res;
}


// Algorithme pour rendre entières les coordonnées de la room
// n°i sans qu'elle rentre en collision avec la room n°lastc
// (et donc normalement aucune des autres).
void intifyRoom(Room** rlist, int i, int lastc) {
    Room* room = rlist[i]; Room* roomc = rlist[lastc];

    float r = room->r;
    float t = room->theta;

    int w = room->w; int wc = roomc->w;
    int h = room->h; int hc = roomc->h;

    int x = (int)(r*cos(t) - (float)w/2.);
    int y = (int)(r*sin(t) - (float)h/2.);

    room->x = x;
    room->y = y;
}


// Calcule la distance entre le centre de deux rooms
float dist(Room* room1, Room* room2) {
    float cx1 = (float)room1->x + (float)room1->w/2.;
    float cy1 = (float)room1->y + (float)room1->h/2.;

    float cx2 = (float)room2->x + (float)room2->w/2.;
    float cy2 = (float)room2->y + (float)room2->h/2.;

    float dx = fabs(cx2 - cx1);
    float dy = fabs(cy2 - cy1);
    
    return sqrt(dx*dx + dy*dy);
}


// Calcule l'angle en radians entre le vecteur (1, 0),
// et le vecteur centreroom1--->centreroom2.
// Le résultat est compris entre 0 et 2*pi.
float angle(Room* room1, Room* room2) {
    float cx1 = (float)room1->x + (float)room1->w/2.;
    float cy1 = (float)room1->y + (float)room1->h/2.;
    
    float cx2 = (float)room2->x + (float)room2->w/2.;
    float cy2 = (float)room2->y + (float)room2->h/2.;

    // Vecteur (1, 0)
    float vx1 = 1.; float vy1 = 0.;
    // Vecteur return -1centreroom1--->centreroom2
    float vx2 = cx2 - cx1; float vy2 = cy2 - cy1;

    // Calcul de l'angle entre -pi et pi
    float ang = atan2(vy2, vx2);
    // Calcul de l'angle entre 0 et 2*pi
    if (ang < 0) {
        float pi = 3.141592654;
        ang = 2*pi + ang;
    }

    return ang;
}


// Crée la liste de proximité de la room n°i de la liste de rooms rlist.
// La liste de proximité d'une room r est la liste des coordonnées
// polaires (où le centre des coordonnées polaires n'est pas 0 mais le centre de r)
// des n rooms les plus proche de cette room r
ProxiList* createProximityList(Room** rlist, int n_room, int i, int n) {
    // Allouer la mémoire nécessaire
    ProxiList* pList = (ProxiList*)malloc(sizeof(ProxiList));
    pList->room = rlist[i];
    Room** proxi = (Room**)malloc(n*sizeof(Room*));
    pList->proxi = proxi;
    for (int j = 0; j < n; j++) {
        proxi[j] = initRoom();
        proxi[j]->r = FLT_MAX;
    }

    // Itérer sur rlist pour trouver les rooms les plus proches, dont on stocke alors l'indice dans proxi[i]->nb
    float max = FLT_MAX;
    float d;
    Room* roomi = rlist[i];
    for (int j = 0; j < n_room; j++) {
        d = dist(roomi, rlist[j]);
        if (d < max && i != j) {
            proxi[n-1]->r = d;
            proxi[n-1]->theta = angle(roomi, rlist[j]);
            proxi[n-1]->nb = rlist[j]->nb;
            sortRooms(proxi, n);
            max = proxi[n-1]->r;
        }
    }

    return pList;
}


// Crée la liste des listes de proximité d'une liste de rooms.
// La liste de proximité d'une room r est la liste des coordonnées
// polaires (où le centre des coordonnées polaires n'est pas 0 mais le centre de r)
// des n rooms les plus proche de cette room r
ProxiList** createProximityLists(Room** rlist, int n_room, int n) {
    // Allocating space
    ProxiList** pLists = (ProxiList**)malloc(n_room*sizeof(ProxiList*));
    
    // Computing proximity lists
    for (int i = 0; i < n_room; i++) {
        pLists[i] = createProximityList(rlist, n_room, i, n);
    }

    return pLists;
}


// Destructeur de ProxiList.
void freeProximityList(ProxiList* list, unsigned int k) {
    for (unsigned int i = 0; i < k; i++) free(list->proxi[i]);
    free(list->proxi);
    free(list);
}


// Destructeur d'une liste de ProxiList
void freeProximityLists(ProxiList** lists, unsigned int n, unsigned int k) {
    for (unsigned int i = 0; i < n; i++) freeProximityList(lists[i], k);
    free(lists);
}


// Calcule le plus petit écart entre le theta de la room n°i et
// celui de n'importe quelle room la précédant
float closestTheta(Room** proxi, int i) {
    float res = FLT_MAX;
    
    float pi = 3.141592654;
    float d;
    if (i != 0) {
        for (int j = 0; j < i; j++) {
            d = fabs(proxi[j]->theta - proxi[i]->theta);
            if (d > pi) {
                d = 2.*pi - d; // L'angle maximal entre deux vecteurs est pi, et non 2*pi.
            }

            if (d < res) {
                res = d;
            }
        }
    }
    return res;
}


// Retourne la minimum entre les entiers x et y
int min(int x, int y) {
    if (x < y) {
        return x;
    } else {
        return y;
    }
}


// Retourne la maximum entre les entiers x et y
int max(int x, int y) {
    if (x < y) {
        return y;
    } else {
        return x;
    }
}


// Enlève les doublons d'une liste de liens
int* removeDuplicates(int* linksList) {
    // Find maximum length and allocate memory
    int len = 0;
    for (int i = 0; linksList[i] != -1; i++) {
        len += 1;
    }
    int* newLinksList = (int*)malloc(len*sizeof(int));
    int wi = 0; // Writing Index

    int k1, k2; // Tuple des indices des rooms linkées
    int foundSame; // int Flag. Set to true whenever current link is present earlier in the new list
    for (int i = 0; linksList[i] != -1; i++, i++) {
        k1 = linksList[i]; k2 = linksList[i+1];
        foundSame = 0;
        for (int j = 0; j < wi; j++, j++) {
            if (k1 == newLinksList[j] && k2 == newLinksList[j+1]) {
                foundSame = 1;
                break;
            }
        }
        if (!foundSame) {
            newLinksList[wi] = k1; wi++;
            newLinksList[wi] = k2; wi++;
        }
    }
    newLinksList[wi] = -1; wi++;

    return newLinksList;
}


// Décide de quelles liaisons maintenir à partir de la liste de proximité proxiList et des hyperparamètres
// Renvoie une liste d'entiers, qui est en réalité la liste des couples des indices des rooms constituant chaque liaison.
int* decideOfLinks(ProxiList** pLists, int n_room, int n_proxi, float max_r, float min_theta) {
    // Allocate memory
    int* linksList = (int*)malloc((2*n_proxi*n_room + 1)*sizeof(int)); // +1 for assured end-of-list NULL character
    for (int i = 0; i < 2*n_room + 1; i++) {
        linksList[i] = 0;
    }

    // Itérer sur la liste des listes de proximité
    int wi = 0; // Writing Index : First index currently writable in the array.
    Room** proxi;
    for (int i = 0; i < n_room; i++) {
        proxi = pLists[i]->proxi;
        // Puis sur la liste de proximité de chaque room
        for (int j = 0; j < n_proxi; j++) {
            // Conditions to creating the link between the rooms
            float ct = closestTheta(proxi, j);
            if ((fabs(proxi[j]->r - proxi[0]->r) <= max_r) && (ct >= min_theta)) {
                // Append link to the list
                linksList[wi] = min(pLists[i]->room->nb, proxi[j]->nb); wi++;
                linksList[wi] = max(pLists[i]->room->nb, proxi[j]->nb); wi++;
            }
        }
    }
    linksList[wi] = -1; wi++;

    int* newLinksList = removeDuplicates(linksList);
    free(linksList);

    return newLinksList;
}


// Checks whether element elem is already present at index i
// or before in the list list.
int notIn(int* list, int elem, int i) {
    if (i == 0) {
        return (list[0] != elem);
    } else if (list[i] == elem) {
        return 0;
    } else {
        return notIn(list, elem, i - 1);
    }
}


// rlist must be the whole list of rooms.
// Prints the tile (i, j) on the map under the condition that
// the tile in (i, j) is free (=-1) or occupied by a small room,
// in which case it appends its number to the smallShow list, at position sswi.
// Returns the new sswi, which is incremented if used.
int conditionPrint(int** map, int n_map, int i, int j, Room** rlist, int* smallShow, int sswi) {
    int tile = map[i + n_map-1][j + n_map-1];
    if (tile == -1) { // tile is empty
        map[i + n_map-1][j + n_map-1] = 2842;
    } else if (tile != 2842) { // tile is a room
        // Check if the room is small
        if (!strcmp(rlist[tile]->type, "small")) {
            if (notIn(smallShow, tile, sswi)) {
                smallShow[sswi] = tile;
                sswi++;
            }
        } else { // else the room is not small, send signal to break the loop
            sswi = -1;
        }
        
    } // else, the tile is a link already

    return sswi;
}

// rlist must be the whole list of rooms.
// Creates one link (potentially two, see gen.txt) between room1 and room2, and prints it on the map.
// tmin and cor_w are hyperparameters, passed respectively for choosing the number of tiles that should
// be left free of links tiles, on the edge of rooms (see draft.png); and the corridor's width.
// Please note that both room's w and h must respect w <= 2*(cor_w + 2*tmin) - 1 >= h.
int printLinkToMap(int** map, int n_map, Room* room1, Room* room2, int tmin, int cor_w, Room** rlist, int* smallShow, int sswi) {
    // Extract data to variables for better readabillity
    int x1 = room1->x; int x2 = room2->x;
    int y1 = room1->y; int y2 = room2->y;

    int w1 = room1->w; int w2 = room2->w;
    int h1 = room1->h; int h2 = room2->h;

    int r1 = x1 + w1; int r2 = x2 + w2; // x coordinates of the right side of rooms
    int t1 = y1 + h1; int t2 = y2 + h2; // y coordinates of the top side of rooms

    // Check for assumption
    int amin = 2*(cor_w + 2*tmin) - 1;
    if (w1 < amin || w2 < amin || h1 < amin || h2 < amin) {
        printf("Warning! One of the room's width of height is too small compared to 2*(cor_w + 2*tmin) - 1 (=%d).\n", amin);
    }

    //////////////////////////////////////////
    // Computes b, which represents the number of tiles that
    // the two rooms have in common, vertically or horizontally.
    // If the rooms are not close on that axis,
    // b represents the number of tiles between the rooms in that axis instead.
    int bx, by;

    // Computes bx
    if (r1 >= x2 && r2 >= x1) { // Case where the rooms' x coordinates match
        bx = min(r1 - x2, r2 - x1); // bx is positive
    } else if (x1 <= x2) { // Else, if room1 is the room on the left
        bx = r1 - x2; // bx is negative
    } else { // Else, room2 is the room on the left
        bx = r2 - x1; // bx is negative
    }

    // Computes by
    if (t1 >= y2 && t2 >= y1) { // Case where the rooms' y coordinates match
        by = min(t1 - y2, t2 - y1); // by is positive
    } else if (y1 <= y2) { // Else, if room1 is the bottom room
        by = t1 - y2; // by is negative
    } else { // Else, room2 is the bottom room
        by = t2 - y1; // by is negative
    }

    //////////////////////////////////////////
    // Computes a, which represents the number of tiles available,
    // horizontally or vertically, for link 1, 2, 3, or 4, to set the link's start into.
    // a must be greater than cor_w for the link to be generated using this end.
    // Also computes o (if needed), which then means, for instance,
    // that x1+ox3 is the beginning of the available space for room1's width for link 3.
    // Link 1 goes vertically, link 2 goes horizontally,
    // link 3 goes from room1's width to room2's height, and link 4 goes from room2's width to room1's height.
    int ax1 = 0;
    int ay2 = 0;
    int ax3 = 0; int ay3 = 0;
    int ax4 = 0; int ay4 = 0;

    // Computes ax's    
    if (bx >= cor_w + 2*tmin) { // Case where we'll build the link vertically (link 1), since there is enough common x space
        ax1 = bx - 2*tmin;
    } else if (bx > 0) { // bx is still positive so there is common x space, but not enough (links 3 & 4)
        ax3 = w1 - bx - 2*tmin;
        ax4 = w2 - bx - 2*tmin;
    } else { // bx is negative, which means there is no common x space (links 3 and 4)
        ax3 = w1 - 2*tmin;
        ax4 = w2 - 2*tmin;
    }

    // Computes ay's
    if (by >= cor_w + 2*tmin) { // Case where we'll build the link horizontally (link 2), since there is enough common y space
        ay2 = by - 2*tmin;
    } else if (by > 0) { // by is still positive so there is common y space, but not enough (links 3 & 4)
        ay3 = h2 - by - 2*tmin;
        ay4 = h1 - by - 2*tmin;
    } else { // by is negative, which means there is no common y space (links 3 and 4)
        ay3 = h2 - 2*tmin;
        ay4 = h1 - 2*tmin;
    }

    //////////////////////////////////////////
    // Decides of the random values of where to actually place the links
    // on the rooms' widths and heights
    int rx1 = 0;
    int ry2 = 0;
    int rx3 = 0; int ry3 = 0;
    int rx4 = 0; int ry4 = 0;

    
    if (ax1 != 0) { // Create link 1 alone
        if (ax1 - cor_w != 0) {
            rx1 = rand()%(ax1 - cor_w);
        }
    } else if (ay2 != 0) { // Create link 2 alone
        if (ay2 - cor_w != 0) {
            ry2 = rand()%(ay2 - cor_w);
        }
    } else { // Create links 3 & 4
        if (ax3 - cor_w != 0) {
            rx3 = rand()%(ax3 - cor_w);
        }
        if (ay3 - cor_w != 0) {
            ry3 = rand()%(ay3 - cor_w);
        }
        if (ax4 - cor_w != 0) {
            rx4 = rand()%(ax4 - cor_w);
        }
        if (ay4 - cor_w != 0) {
            ry4 = rand()%(ay4 - cor_w);
        }
    }

    //////////////////////////////////////////
    // Determines xc and yc for every corridor that is about to be created
    // For corridors 1 or 2, xc and yc are the coordinates of the bottom-left corner of the corridor
    // For corridors 3 & 4, xc and yc are the coordinates of the farthest corner of the corridor's angle.
    int xc1 = 0; int yc1 = 0;
    int xc2 = 0; int yc2 = 0;
    int xc3 = 0; int yc3 = 0;
    int xc4 = 0; int yc4 = 0;
    
    if (ax1 != 0) { // Create link 1 alone
        // xc1
        if (x1 < x2) { // room1 is the left room
            xc1 = x2 + tmin + rx1;
        } else {
            xc1 = x1 + tmin + rx1;
        }
        // yc1
        if (y1 < y2) { // room1 is the bottom room
            yc1 = t1;
        } else {
            yc1 = t2;
        }

    } else if (ay2 != 0) { // Create link 2 alone
        // xc2
        if (x1 < x2) { // room1 is the left room
            xc2 = r1;
        } else {
            xc2 = r2;
        }
        // yc2
        if (y1 < y2) { // room1 is the bottom room
            yc2 = y2 + tmin + ry2;
        } else {
            yc2 = y1 + tmin + ry2;
        }

    } else { // Create links 3 & 4
        // Link 3
        // Case disjonction on the angle's placement
        if (x1 < x2 && y1 < y2) { // Angle is top-left
            xc3 = x1 + tmin + rx3;
            yc3 = y2 + tmin + ry3 + cor_w + max(0, by);
        } else if (x1 < x2 && y2 < y1) { // Angle is bottom-left
            xc3 = x1 + tmin + rx3;
            yc3 = y2 + tmin + ry3;
        } else if (x2 < x1 && y2 < y1) { // Angle is bottom-right
            xc3 = x1 + tmin + rx3 + cor_w + max(0, bx);
            yc3 = y2 + tmin + ry3;
        } else { // Angle is top-right
            xc3 = x1 + tmin + rx3 + cor_w + max(0, bx);
            yc3 = y2 + tmin + ry3 + cor_w + max(0, by);
        }

        // Link 4
        // Case disjonction on the angle's placement
        if (x1 < x2 && y1 < y2) { // Angle is bottom-right
            xc4 = x2 + tmin + rx4 + cor_w + max(0, bx);
            yc4 = y1 + tmin + ry4;
        } else if (x1 < x2 && y2 < y1) { // Angle is top-right
            xc4 = x2 + tmin + rx4 + cor_w + max(0, bx);
            yc4 = y1 + tmin + ry4 + cor_w + max(0, by);
        } else if (x2 < x1 && y2 < y1) { // Angle is top-left
            xc4 = x2 + tmin + rx4;
            yc4 = y1 + tmin + ry4 + cor_w + max(0, by);
        } else { // Angle is bottom-left
            xc4 = x2 + tmin + rx4;
            yc4 = y1 + tmin + ry4;
        }
    }

    //////////////////////////////////////////
    // Draws the link(s) on the map
    int tsswi = 0; // test sswi
    if (ax1 != 0) { // Draw link 1 alone
        for (int j = xc1; j < xc1 + cor_w; j++) {
            for (int i = yc1; i < max(y1, y2); i++) {
                tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                if (tsswi == -1) {
                    break;
                } else {
                    sswi = tsswi;
                }
            }
        }

    } else if (ay2 != 0) { // Draw link 2 alone
        for (int i = yc2; i < yc2 + cor_w; i++) {
            for (int j = xc2; j < max(x1, x2); j++) {
                tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                if (tsswi == -1) {
                    break;
                } else {
                    sswi = tsswi;
                }
            }
        }

    } else { // Draw links 3 & 4
        // Link 3
        // Case disjonction on the angle's placement
        if (x1 < x2 && y1 < y2) { // Angle is top-left
            // Horizontal link
            for (int i = yc3; i > yc3 - cor_w; i--) {
                for (int j = xc3; j < x2; j++) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
            // Vertical link
            for (int j = xc3; j < xc3 + cor_w; j++) {
                for (int i = yc3; i > t1 - 1; i--) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }

        } else if (x1 < x2 && y2 < y1) { // Angle is bottom-left
            // Horizontal link
            for (int i = yc3; i < yc3 + cor_w; i++) {
                for (int j = xc3; j < x2; j++) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
            // Vertical link
            for (int j = xc3; j < xc3 + cor_w; j++) {
                for (int i = yc3; i < y1; i++) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }

        } else if (x2 < x1 && y2 < y1) { // Angle is bottom-right
            // Horizontal link
            for (int i = yc3; i < yc3 + cor_w; i++) {
                for (int j = xc3; j > r2 - 1; j--) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
            // Vertical link
            for (int j = xc3; j > xc3 - cor_w; j--) {
                for (int i = yc3; i < y1; i++) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }

        } else { // Angle is top-right
            // Horizontal link
            for (int i = yc3; i > yc3 - cor_w; i--) {
                for (int j = xc3; j > r2 - 1; j--) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
            // Vertical link
            for (int j = xc3; j > xc3 - cor_w; j--) {
                for (int i = yc3; i > t1 - 1; i--) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
        }


        // Link 4
        // Case disjonction on the angle's placement
        if (x1 < x2 && y1 < y2) { // Angle is bottom-right
            // Horizontal link
            for (int i = yc4; i < yc4 + cor_w; i++) {
                for (int j = xc4; j > r1 - 1; j--) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
            // Vertical link
            for (int j = xc4; j > xc4 - cor_w; j--) {
                for (int i = yc4; i < y2; i++) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }

        } else if (x1 < x2 && y2 < y1) { // Angle is top-right
            // Horizontal link
            for (int i = yc4; i > yc4 - cor_w; i--) {
                for (int j = xc4; j > r1 - 1; j--) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
            // Vertical link
            for (int j = xc4; j > xc4 - cor_w; j--) {
                for (int i = yc4; i > t2 - 1; i--) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }

        } else if (x2 < x1 && y2 < y1) { // Angle is top-left
            // Horizontal link
            for (int i = yc4; i > yc4 - cor_w; i--) {
                for (int j = xc4; j < x1; j++) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
            // Vertical link
            for (int j = xc4; j < xc4 + cor_w; j++) {
                for (int i = yc4; i > t2 - 1; i--) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }

        } else { // Angle is bottom-left
            // Horizontal link
            for (int i = yc4; i < yc4 + cor_w; i++) {
                for (int j = xc4; j < x1; j++) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
            // Vertical link
            for (int j = xc4; j < xc4 + cor_w; j++) {
                for (int i = yc4; i < y2; i++) {
                    tsswi = conditionPrint(map, n_map, i, j, rlist, smallShow, sswi);
                    if (tsswi == -1) {
                        break;
                    } else {
                        sswi = tsswi;
                    }
                }
            }
        }
    }

    return sswi;
}


// Prints all links from a linksList to a map
// Returns the list of small rooms to be displayed
int* printAllLinksToMap(int** map, int n_map, Room** rlist, int* linksList, int tmin, int cor_w, int n_small) {
    int sswi = 0;
    int* smallShow = (int*)calloc(n_small+1, sizeof(int));
    for (int i = 0; linksList[i] != -1; i++, i++) {
        sswi = printLinkToMap(map, n_map, rlist[linksList[i]], rlist[linksList[i+1]], tmin, cor_w, rlist, smallShow, sswi);
    }
    smallShow[sswi] = -1; sswi++;

    return smallShow;
}



// Renvoie les lignes/colonnes min/max dans lequels il y a des valeurs. 
// Laise une colonne et une ligne d'écart entre le bort et les pièces.
// Hyp : le contenu de rooms ne touche pas les bords du tableau.
void getRoomsXYMinMaxWish(int** map, int size, unsigned int* x_min, unsigned int* x_max,
    unsigned int* y_min, unsigned int* y_max) {
    
    // Met à zéro les valeurs des bornes.
    *x_max = 0;
    *y_max = 0;
    *x_min = size;
    *y_min = size;

    // Parcours du tableau pour déterminer les bonnes bornes.
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            if (map[j][i] == -1) continue;
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


Rooms* generateRooms(HyperParam* hyper, LevelEntity* momie, List* entities, TileSet* ennemi_map_tileset) {
    int n_large = hyper->n_large;
    int n_small = hyper->n_small;
    int n_lhidden = hyper->n_lhidden;
    int n_shidden = hyper->n_shidden;
    float dr = hyper->dr;
    int n_map = hyper->n_map;
    int n_proxi = hyper->n_proxi;
    float max_r = hyper->max_r;
    float min_theta = hyper->min_theta;
    int cor_w = hyper->corridor_width;
    int tmin = hyper->tile_gap;
    int xstairs = hyper->xStairs;
    int ystairs = hyper->yStairs;

    // Génère une liste de Room.
    int n_room = n_large + n_small + n_lhidden + n_shidden;
    Room** r_list = (Room**)malloc(n_room*sizeof(Room*));
    int i = 0;
    int j = 0;
    // Large rooms
    for (j = 0; j < n_large; i++, j++) {
        r_list[i] = createRoom("large", 10, 15, 9, 13);
    }
    // Small rooms
    for (j = 0; j < n_small; i++, j++) {
        r_list[i] = createRoom("small", 4, 5, 4, 5);
    }
    // Vertical corridors
    for (j = 0; j < n_lhidden; i++, j++) {
        r_list[i] = createRoom("lhidden", 10, 15, 9, 13);
    }
    // Horizontal corridors
    for (j = 0; j < n_shidden; i++, j++) {
        r_list[i] = createRoom("shidden", 4, 5, 4, 5);
    }

    // Trier les rooms par rayon croissant
    sortRooms(r_list, n_room);

    // Dans la boucle qui suit, on fixe la room i, en l'éloignant
    // de "dr" jusqu'à ce qu'elle ne rentre plus en collision avec
    // aucune des autres rooms fixées jusque là.
    // On trouve ensuite ses coordonnées entières cartésiennes x et y
    int c; // Résultat de collides(r_list, i)
    int lastc = 0; // Avant-dernier résultat de c en fin de boucle = Indice de la dernière room à avoir collisionné avec la room r_list[i]
    for (i = 0; i < n_room; i++) {
        r_list[i]->nb = i; // Remplir les cases nb des rooms

        c = 0;
        while (c != -1) {
            r_list[i]->r += dr;
            lastc = c;
            c = collides(r_list, i);
        }
        
        // Trouver les coordonnées entières cartésiennes de la room
        intifyRoom(r_list, i, lastc);
    }

    // Initialiser la liste des rooms large
    Room** l_rooms = (Room**)calloc(n_large+1, sizeof(Room*));
    for (i = 0, j = 0; i < n_room; i++) {
        if (!strcmp(r_list[i]->type, "large")) {
            l_rooms[j] = r_list[i];
            j++;
        }
    }

    // Créer la liste des listes de proximité
    ProxiList** pLists = createProximityLists(l_rooms, n_large, n_proxi);
    // Trouver la room large la plus proche de (xstairs, ystairs) pour le spawn
    Room* stairsPos = initRoom();
    stairsPos->x = xstairs; stairsPos->y = ystairs;
    if (l_rooms[n_large]) freeRoom(l_rooms[n_large]);
    l_rooms[n_large] = stairsPos;
    ProxiList* prx_lst = createProximityList(l_rooms, n_large + 1, n_large, 1);
    Room* stairsRoom = r_list[prx_lst->proxi[0]->nb]; // Pointeur vers la large room dans laquelle placer les escaliers d'arrivée
    freeProximityList(prx_lst, 1);

    // Décider des liaisons à maintenir
    int* linksList = decideOfLinks(pLists, n_large, n_proxi, max_r, min_theta);

    // Initialiser la map
    Rooms* rooms = createRooms(n_map*2);

    // Remplir la map en itérant sur les rooms
    Room* room;
    int x; int y;
    int w; int h;
    int nb;
    int k;
    for (k = 0; k < n_room; k++) {
        room = r_list[k];
        if (!strcmp(room->type, "large") || !strcmp(room->type, "small")) {
            x = room->x; y = room->y;
            w = room->w; h = room->h;
            nb = room->nb;

            // Itération sur les tiles de la room
            for (i = y; i < y + h; i++) {
                for (j = x; j < x + w; j++) {
                    if (i + n_map-1 < 0  || i + n_map-1 >= 2*n_map || j + n_map-1 < 0 || j + n_map-1 >= 2*n_map) {
                        printf("Your N_MAP hyperparameter (=%d) is too small.\n\n", n_map);
                        abort();
                    } else {
                        rooms->map[i + n_map-1][j + n_map-1] = nb;
                    
                    }
                }
            }
        }
    }

    // Dessiner les chemins.
    int* smallShow = printAllLinksToMap(rooms->map, n_map, r_list, linksList, tmin, cor_w, n_small);
    int nSmallShow = 0;
    for (i = 0; smallShow[i] != -1; i++) {
        nSmallShow++;
    }

    // Supprime les petites pièces non reliées aux chemins.
    for (i = 2*n_map-1; i >= 0; i--) {
        for (j = 0; j < 2*n_map; j++) {
            if (rooms->map[i][j] != 2842 && rooms->map[i][j] != -1 &&
                strcmp(r_list[rooms->map[i][j]]->type, "large") && 
                notIn(smallShow, rooms->map[i][j], nSmallShow)) rooms->map[i][j] = -1;
        }
    }

    // Ajoute l'escalier de spawn si l'étage n'est pas le premier :
    int xStairs = stairsRoom->x+stairsRoom->w/2-1; int yStairs = stairsRoom->y+stairsRoom->h/2-2;
    if (hyper->floor_ != 0) {
        for (j = xStairs; j < xStairs+2; j++) {
            for (i = yStairs; i < yStairs+3; i++) {
                rooms->map[i + n_map - 1][j + n_map - 1] = 2942;
            }
        }
    }


    // Ajoute l'escalier pour descendre
    // Choisir une room large random, sauf celle qui contient l'escalier de spawn
    Room* downStairsRoom = l_rooms[rand()%n_large];
    while (downStairsRoom->nb == stairsRoom->nb) {
        downStairsRoom = l_rooms[rand()%n_large];
    }
    // Place les tiles de l'escalier
    x = downStairsRoom->x; y = downStairsRoom->y;
    w = downStairsRoom->w; h = downStairsRoom->h;
    for (j = x+w/2-1; j < x+w/2+1; j++) {
        for (i = y+h/2-2; i < y+h/2+1; i++) {
            rooms->map[i + n_map - 1][j + n_map - 1] = 2578;
        }
    }
    rooms->xDownStairs = x+w/2-1; rooms->yDownStairs = y+h/2-2;


    // Définition de la position initiale de la momie
    unsigned int x_min;
    unsigned int x_max;
    unsigned int y_min;
    unsigned int y_max;
    getRoomsXYMinMaxWish(rooms->map, rooms->size, &x_min, &x_max, &y_min, &y_max);
    int indice_strating_room= rand()%n_large;
    Room* starting_room=l_rooms[indice_strating_room];
    int starting_position_x = xStairs;
    int starting_position_y = yStairs + 2;
    momie -> x =rooms->size/2 - x_min +  starting_position_x;
    momie -> y =rooms->size/2 - y_min + starting_position_y;
    listAdd(entities,momie);
 
    // Free des structures de données utilisés pour la génération.
    freeProximityLists(pLists, n_large, n_proxi);
    freeRoomList(r_list, n_room);
    free(linksList);
    free(smallShow);
    free(l_rooms);
    freeRoom(stairsPos);

    // Renvoie le résultat.
    return rooms;

}


// Retourne true si le graphe est connexe
int checkConnexite(Rooms* rooms) {
    // Find any floor tile to start the algorithm
    int x0 = rooms->size/2; int y0 = rooms->size/2;
    while (rooms->map[y0][x0] == -1) {
        if (y0 == rooms->size-1 && x0 == rooms->size-1) {
            x0 = 0;
            y0 = 0;
        } else if (x0 == rooms->size-1) {
            x0 = 0;
            y0 += 1;
        } else {
            x0 += 1;
        }
    }

    // Store a copy of the map into mapCopy
    int** mapCopy = (int**)malloc(rooms->size*sizeof(int*));
    for (int j = 0; j < rooms->size; j++) {
        mapCopy[j] = (int*)malloc(rooms->size*sizeof(int));
        for (int i = 0; i < rooms->size; i++) {
            mapCopy[j][i] = rooms->map[j][i];
        }
    }


    // Initialize the stack with those coordinates
    Pile* pile = createEmptyPile();
    pile = appendPile(pile, x0, y0);
    while (!isEmptyPile(pile)) {
        // Get the new tile coordinates
        int x = pile->x; int y = pile->y;
        // Update the stack without the top tile
        pile = popPile(pile);
        
        // Check that current tile isn't yet visited to avoid infinite loops
        if (mapCopy[y][x] != -1) {
            // Check the 4 tiles reachable from (x, y) and add them to the stack if they're ground and not yet visited
            if (mapCopy[y][x+1] != -1) {
                pile = appendPile(pile, x+1, y);
            }
            if (mapCopy[y-1][x] != -1) {
                pile = appendPile(pile, x, y-1);
            }
            if (mapCopy[y][x-1] != -1) {
                pile = appendPile(pile, x-1, y);
            }
            if (mapCopy[y+1][x] != -1) {
                pile = appendPile(pile, x, y+1);
            }
        }

        // Mark the current tile as visited
        mapCopy[y][x] = -1;
    }


    // Iterate over the whole map to search for an unvisited tile
    int connexe = 1;
    for (int j = 0; j < rooms->size; j++) {
        for (int i = 0; i < rooms->size; i++) {
            if (mapCopy[j][i] != -1) {
                connexe = 0;
                goto breakLoops;
            }
        }
    }
    breakLoops:

    // Free the map copy
    for (int j = 0; j < rooms->size; j++) {
        free(mapCopy[j]);
    }
    free(mapCopy);
    freePile(pile);
    return connexe;
}
