/* aglorithme comes from https://www.geeksforgeeks.org/binary-heap/
and https://www.geeksforgeeks.org/heap-sort/ */


#ifndef __BINHEAP_H__
#define __BINHEAP_H__

#include "base.h"

typedef struct Nodes_s Nodes_t;

struct Nodes_s {
    int nodeId;
    my_m1 m1;
    my_m2 m2;
};

typedef struct BinHeap_s BinHeap_t;

/**
 * @brief This structure represent a stack in which are
 * active nodes, for the dijkstra computing
 */

struct BinHeap_s {
    Nodes_t* keys;
    int heapSize;
    int maxSize;
    int* isPresent;
};

/**
 * @brief Initialize à new heap
 * 
 * @param bp            binary heap to init
 * @param size          size of the given heao
 */

void BinHeap_init(BinHeap_t* bp, int size);


/**
 * @brief return the index parent of element at index i
 * 
 * @param i             index
 * 
 * @return return the parent index
 */

int parent(int i); 


/**
 * @brief return the index of the left child
 * 
 * @param i             index
 * 
 * @return return the left child index
 */

int left(int i);
 

/**
 * @brief return the index of the right child
 * 
 * @param i             index
 * 
 * @return return the left right index
 */

int right(int i);


/**
 * @brief swap the two values x and y
 * 
 * @param x             first value
 * @param y             second value
 */

void swap(Nodes_t* x, Nodes_t* y);


/**
 * @brief swap the two int values x and y
 * 
 * @param x             first value
 * @param y             second value
 */

void swap_int(int* x, int* y);


/**
 * @brief Insert a key at the bottom of the heap, and the heapify
 * the heap
 * 
 * @param bp            binary heap
 * @param node          node to add
 * @param m1            delay of the added element
 * @param m2            igp weight of the added element
 */

void BinHeap_insert_key(BinHeap_t* bp, int node, my_m1 m1, my_m2 m2);


/**
 * @brief heapify (sort) the binary heap
 * 
 * @param bp            binary heap
 * @param i             index of the first element (0)
 */

void BinHeap_min_heapify(BinHeap_t* bp, int i);


/**
 * @brief extract the lowest element from the heap and then
 * heapify the heap
 * 
 * @param bp            binary heap
 * 
 * @return return the value (node) of the lowest element in the heap
 */

int BinHeap_extract_min(BinHeap_t* bp);

void BinHeap_free(BinHeap_t* bp);

void BinHeap_print(BinHeap_t* bp);


/**
 * @brief Set m1 and m2 to new m1 and m1 of the element having
 * el.node == node
 * 
 * @param bp            binary heap
 * @param node          node value of the element
 * @param m1            new delay
 * @param m2            new igp weight
 */

void BinHeap_decrease_key(BinHeap_t* bp, int node, my_m1 m1, my_m2 m2);


#endif