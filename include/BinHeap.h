/* aglorithme comes from https://www.geeksforgeeks.org/binary-heap/
and https://www.geeksforgeeks.org/heap-sort/ */


#ifndef __BINHEAP_H__
#define __BINHEAP_H__

#include "base.h"


typedef struct BinHeap_s BinHeap_t;

/**
 * @brief This structure represent a stack in which are
 * store all the active indexes, means all the indexes for which it exists
 * a path in the correponding dictionary
 */

struct BinHeap_s {
    my_m1* keys;
    int heapSize;
    int maxSize;
};

/**
 * @brief Initialize à new heap
 * 
 * @param bp            binary heap to init
 * @param size          size of the given heao
 */

void BinHeap_init(BinHeap_t* bp, int size);


/**
 * @brief insert a new index in the heap if there is
 * still place in
 * 
 * @param bp            binary heap
 * @param key           value on the new active index
 */

void BinHeap_insert_key(BinHeap_t* bp, my_m1 key);


/**
 * @brief sort the heap (Quick Sort)
 */

void BinHeap_sort(BinHeap_t* bp);


/**
 * @brief Only use for debug
 */

void BinHeap_print(BinHeap_t* bp);


BinHeap_t* BinHeap_merge_sort(BinHeap_t* pfront, BinHeap_t* pfcand);


void BinHeap_free(BinHeap_t* bh);

#endif