#include "../include/ScopeStack.h"



myStack* initStack(int nbNode)
{
    myStack* stack = malloc(sizeof(myStack));
    ASSERT(stack, NULL);
    stack->stackSize = 0;

    stack->maxSize = nbNode;
    stack->isPresent = malloc(nbNode * sizeof(bool));
    for (int i = 0 ; i < nbNode ; i++) {
        stack->isPresent[i] = false;
    }
    stack->stack = malloc(nbNode * sizeof(int));

    return stack;
}


int getTop(myStack* stack)
{
    if (stack->stackSize <= 0) {
        return -1;
    }

    return stack->stack[stack->stackSize - 1];
}


int depile(myStack* stack)
{
    if (stack->stackSize <= 0) {
        return -1;
    }

    stack->stackSize--;
    int node = stack->stack[stack->stackSize];
    stack->isPresent[node] = false;
    return node;
}


void empile(int node, myStack* stack)
{
    if (stack->isPresent[node]) {
        return;
    }

    if (stack->stackSize >= stack->maxSize) {
        return;
    }
    stack->isPresent[node] = true;
    stack->stack[stack->stackSize++] = node;
    
}


void display_stack(myStack* stack)
{
    for (int i = 0 ; i < stack->stackSize ; i++) {
        printf("%d\t", stack->stack[i]);
    }
    printf("\nstack size : %d\n", stack->stackSize);
}


void freeStack(myStack* st)
{
    free(st->isPresent);
    free(st->stack);
    free(st);
}