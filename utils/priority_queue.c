#include "priority_queue.h"

#include <stdlib.h>





// Constructeur de PriorityNode.
PriorityNode* createPriorityNode(void* value, float key, PriorityNode* next) {
    PriorityNode* node = (PriorityNode*)malloc(sizeof(PriorityNode));
    node->value = value;
    node->key = key;
    node->next = next;
    return node;
}



// Desstructeur de PriorityNode.
void freePriorityNode(PriorityNode* node) {
    free(node);
}





// Constructeur de PriorityQueue.
PriorityQueue* createPriorityQueue() {
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    queue->first = NULL;
    queue->size = 0;
    return queue;
}



// Destructeur de PriorityQueue.
void freePriorityQueue(PriorityQueue* queue) {
    while (queue->first != NULL) {
        PriorityNode* node = queue->first->next;
        freePriorityNode(queue->first);
        queue->first = node;
    }
    free(queue);
}





// Renvoie la taille de la queue.
int priorityQueueSize(PriorityQueue* queue) {
    return queue->size;
}



// Insère une élément dans la queue.
void priorityQueueInsert(PriorityQueue* queue, void* value, float key) {
    PriorityNode* node = createPriorityNode(value, key, NULL);
    
    // Insère l'élément.
    if (queue->first == NULL) {
        queue->first = node;
    } else {
        node->next = queue->first;
        queue->first = node;
    }    

    // Incrémente la taille.
    queue->size++;

}



// Récupère l'élément de priorité minimale et le supprime de la queue.
void* priorityQueuePop(PriorityQueue* queue) {
    if (queue->first == NULL) return NULL;

    // Initialise les variables pour la recherche du minimum.
    float min_key = queue->first->key;
    PriorityNode* min_value_prev = NULL;
    PriorityNode* min_value = queue->first;

    // Parcourt la queue à la recherche du minimum.
    PriorityNode* prev = NULL;
    PriorityNode* node = queue->first;
    while (node != NULL) {
        if (min_key > node->key) {
            min_key = node->key;
            min_value_prev = prev;
            min_value = node;
        }
        prev = node;
        node = node->next;
    }

    // Réaffecte les pointeurs pour supprimer la cellule du minimum.
    if (min_value_prev != NULL) {
        min_value_prev->next = min_value->next;
    } 
    if (min_value_prev == NULL) {
        queue->first = min_value->next;
    }

    // Dimonue la taille de la queue, free la cellule et renvoie sa valeur.
    queue->size--;
    void* value = min_value->value;
    freePriorityNode(min_value);
    return value;

}
