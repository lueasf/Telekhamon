#pragma once





// Structure de liste simple.
// cap : taille max de la liste.
// size : nombre d'éléments dans la liste.
// data : données de la liste. 
struct s_List {
    unsigned int cap;
    unsigned int size;
    void** data;
};
typedef struct s_List List;



// Constructeur, destructeur.
List* createList(unsigned int cap);
void freeList(List* list);



// Fonctions d'ajout, de suppression et de recherche.
void listAdd(List* list, void* elt);
void listRemove(List* list, unsigned int i);
unsigned int listFind(List* list, void* elt);
void* listGet(List* list, unsigned int i);
void listSet(List* list, unsigned int i, void* elt);
unsigned int listSize(List* list);
void listClear(List* list);



// Fonction de trie de la liste.
void listSort(List* list, int (*cmp)(const void*, const void*));
