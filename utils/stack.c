#include "stack.h"

#include <stdlib.h>





// Constructeur de StackCell.
StackCell* createStackCell(void* value, StackCell* next) {
    StackCell* cell = (StackCell*)malloc(sizeof(StackCell));
    cell->value = value;
    cell->next = next;
    return cell;
}



// Destructeur de StackCell.
void freeStackCell(StackCell* cell) {
    free(cell);
}





// Constructeur de Stack.
Stack* createStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->top = NULL;
    stack->size = 0;
    return stack;
}



// Destructeur de Stack.
void freeStack(Stack* stack) {
    while (stack->top != NULL) {
        StackCell* cell = stack->top->next;
        freeStackCell(stack->top);
        stack->top = cell;
    }
    free(stack);
}





// Push une valeur dans la pile.
void stackPush(Stack* stack, void* value) {
    stack->top = createStackCell(value, stack->top);
    stack->size++;
}



// Pop une valeur depuis la pile.
void* stackPop(Stack* stack) {
    if (stack->top == NULL) return NULL; 
    StackCell* cell = stack->top;
    stack->top = cell->next;
    void* value = cell->value;
    freeStackCell(cell);
    stack->size--;
    return value;
}



// Renvoie la taille de la pile.
int stackSize(Stack* stack) {
    return stack->size;
}
