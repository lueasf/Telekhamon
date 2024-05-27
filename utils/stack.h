#pragma once





// Structure pour une cellule de la pile.
struct s_StackCell {
    void* value;
    struct s_StackCell* next;
};
typedef struct s_StackCell StackCell;



// Structure pour une pile.
struct s_Stack {
    StackCell* top;
    int size;
};
typedef struct s_Stack Stack;





// Constructeur, destructeur.
StackCell* createStackCell(void* value, StackCell* next);
void freeStackCell(StackCell* cell);



// Constructeur, destructeur.
Stack* createStack();
void freeStack(Stack* stack);



// Fonctions de manipulation de la pile.
void stackPush(Stack* stack, void* value);
void* stackPop(Stack* stack);
int stackSize(Stack* stack);
