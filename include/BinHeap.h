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


int parent(int i); 
  
 
int left(int i);
  

int right(int i);

void swap(Nodes_t* x, Nodes_t* y);

void swap_int(int* x, int* y);

void BinHeap_insert_key(BinHeap_t* bp, int node, my_m1 m1, my_m2 m2);

void BinHeap_min_heapify(BinHeap_t* bp, int i);

int BinHeap_extract_min(BinHeap_t* bp);

void BinHeap_free(BinHeap_t* bp);

void BinHeap_print(BinHeap_t* bp);

void BinHeap_decrease_key(BinHeap_t* bp, int node, my_m1 m1, my_m2 m2);


#endif