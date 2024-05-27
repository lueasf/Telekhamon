#include "list.h"

#include <SDL2/SDL.h>
#include <stdlib.h>





// Constructeur de List.
List* createList(unsigned int cap) {
    if (cap == 0) return NULL;
    
    // Initialise la list.
    List* list = (List*)malloc(sizeof(List));
    if (!list) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour List");
        abort();
    }
    list->cap = cap;
    list->size = 0;

    // Initialise les données et renvoie la liste.
    list->data = (void**)calloc(cap, sizeof(void*));
    if (!list->data) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de malloc de la mémoire pour List->data");
        abort();
    }
    return list;

}



// Destructeur de List.
void freeList(List* list) {
    if (!list) return;
    free(list->data);
    free(list);
}





// Fonction d'ajout en fin de liste.
void listAdd(List* list, void* elt) {
    
    // Double la taille de la liste si elle est pleine.
    if (list->size == list->cap) {
        list->data = (void**)realloc(list->data, 2*list->cap*sizeof(void*));
        for (unsigned int i = list->cap; i < 2*list->cap; i++) list->data[i] = 0;
        list->cap *= 2;
    }

    // Ajoute l'élément.
    list->data[list->size] = elt;
    list->size++;

}



// Fonction de suppression en un indice de la liste.
void listRemove(List* list, unsigned int i) {
    if (list->size > 0) {
        for (unsigned int j = i+1; j < list->size; j++) list->data[j-1] = list->data[j];
        list->data[list->size-1] = 0;
        list->size--;
    }

}



// Recherche un élément dans la liste.
unsigned int listFind(List* list, void* elt) {
    for (unsigned int i = 0; i < list->size; i++)
        if (list->data[i] == elt) return i;
    return list->size;
}



// Getter.
void* listGet(List* list, unsigned int i) {
    return list->data[i];
}



// Setter.
void listSet(List* list, unsigned int i, void* elt) {
    list->data[i] = elt;
}



// Renvoie la taille de la liste.
unsigned int listSize(List* list) {
    return list->size;
}



// Vide la liste.
void listClear(List* list) {
    list->size = 0;
}





// Permet de trier la liste selon l'ordre cmp.
void listSort(List* list, int (*cmp)(const void*, const void*)) {
    qsort(list->data, list->size, sizeof(void*), cmp);
}
