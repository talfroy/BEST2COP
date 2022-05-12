#ifndef __SCOPESTACK_H__
#define __SCOPESTACK_H__

#include "base.h"

/**
 * @brief Struct for a stack
 * 
 * This structure is only used in dijkstra function
 */

typedef struct {
    int* stack;
    int stackSize;
    int maxSize;
    bool* isPresent;
} myStack;



/**
 * @brief initialise an empty stack
 * 
 * @return return the empty stack
 **/

myStack* initStack(int nbNode);


/**
 * @brief get the top of the stack
 * 
 * @param stack         stack
 * 
 * @return return the node in the top of the stack
 **/

int getTop(myStack* stack);


/**
 * @brief pop the top of the stack
 * 
 * @param stack         stack
 **/

int depile(myStack* stack);


/**
 * @brief put the new node at the top of the stack
 * 
 * @param stack         stack
 * @param node          node Id
 **/

void empile(int node, myStack* stack);

/**
 * @brief Only use for debug
 */

void display_stack(myStack* stack);


void freeStack(myStack* st);

#endif