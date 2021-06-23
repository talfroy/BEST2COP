#ifndef __DICT_H__
#define __DICT_H__

#include "Llist.h"
#include "base.h"
#include "Extendable.h"
#include "BinHeap.h"



/**
 * @brief this structure represent a Dictionnary in which
 * all the paths will be stored
 * 
 * In this structure, the index in paths represent the first component
 * value. For example paths[10].value = 20 means there exists a path
 * (10 ; 20).
 */

struct Dict_s {
    my_m2* paths;             /**< all the existing paths */
    short* preds;
    int size;               /**< max size of the Dictionnary */
    int nb_add;
};

typedef struct Dict_s Dict_t;

/**
 * @brief create a new empty Dictionary
 */

extern Dict_t* Dict_new();


/**
 * @brief allocate a Dictionary and set it to 0
 * 
 * @param dic           Dictionary to allocate
 * @param size          max size of the Dictionary
 * @param heapsize      size oh the dictionary's heaps
 * @param realSpread    real spread in the SR Graph
 */

extern void Dict_init(Dict_t* dic, int size);



/**
 * @brief add a new path in a Dictionary
 * 
 * @param dic           Dictionary
 * @param key           first component of the path (delay)
 * @param value         second component of the path (IGP weight)
 * @param nbSeg         number of segment needed to build
 * this path
 */

extern void Dict_add(Dict_t* dic, my_m1 key, my_m2 value, short pred);


/**
 * @brief Only use for debug
 */

extern void Dict_print(Dict_t* dic);


extern void Dict_free(Dict_t* dic);


/**
 * @brief Reset all the values in path component of dict, i.e INF
 * 
 * @param d             Dictionnary to reset
 */

extern void Dict_reset(Dict_t* d);


#endif