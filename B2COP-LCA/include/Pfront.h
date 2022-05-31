/* aglorithme comes from https://www.geeksforgeeks.org/binary-heap/
and https://www.geeksforgeeks.org/heap-sort/ */


#ifndef __PFRONT_H__
#define __PFRONT_H__

#include "base.h"


typedef struct Pfront_s Pfront_t;

/**
 * @brief This structure represent a stack in which are
 * store all the active indexes, means all the indexes for which it exists
 * a path in the correponding dictionary
 */

struct Pfront_s {
    my_m1** keys;
    int* heapSize;
    int maxSize;
};

/**
 * @brief Initialize à new heap
 * 
 * @param bp            binary heap to init
 * @param size          size of the given heao
 */

void Pfront_init(Pfront_t* bp, int size);


/**
 * @brief insert a new index in the heap if there is
 * still place in
 * 
 * @param bp            binary heap
 * @param key           value on the new active index
 */

void Pfront_insert_key(Pfront_t* bp, my_m1 key_m1, my_m0 key_m0);

/**
 * @brief sort the heap (Quick Sort)
 */

void Pfront_sort(Pfront_t* bp);


/**
 * @brief Only use for debug
 */

void Pfront_print(Pfront_t* pf, FILE* out);


/**
 * @brief Unused for now
 */

Pfront_t* Pfront_merge_sort(Pfront_t* pfront, Pfront_t* pfcand);


void Pfront_free(Pfront_t* bh);


/**
 * @brief check if it exists a path in pfront[dst] before
 * the MSDth iteration
 * 
 * @param pfront            stack to check
 * @param dst               destination node
 * 
 * @return return true if it exists a path before the MSDth iteration,
 * false if not
 */

bool hasapath(Pfront_t** pfront, int dst);

#endif