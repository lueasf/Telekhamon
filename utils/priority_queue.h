#pragma once





// Structure pour une cellule de la file.
struct s_PriorityNode {
    void* value;
    float key;
    struct s_PriorityNode* next;
};
typedef struct s_PriorityNode PriorityNode;



// Structure pour la pile.
struct s_PriorityQueue {
    int size;
    PriorityNode* first;
};
typedef struct s_PriorityQueue PriorityQueue;





// Constructeur, destructeur.
PriorityNode* createPriorityNode(void* value, float key, PriorityNode* next);
void freePriorityNode(PriorityNode* node);



// Constructeur, destructeur.
PriorityQueue* createPriorityQueue();
void freePriorityQueue(PriorityQueue* queue);



// Fonctions de manipulation de la queue.
int priorityQueueSize(PriorityQueue* queue);
void priorityQueueInsert(PriorityQueue* queue, void* value, float key);
void* priorityQueuePop(PriorityQueue* queue);
