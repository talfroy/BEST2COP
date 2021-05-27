#ifndef __DICT_H__
#define __DICT_H__

#include "Llist.h"
#include "base.h"
#include "Extendable.h"
#include "BinHeap.h"



#define EXTREMITY_PRUNING_OFF   0
#define EXTREMITY_PUNNING_ON    1

#define MINIMAL                 1
#define LAZY                    2


/**
 * @brief this structure represents a 
 * association between a key (igp weight)
 * and a value (complexe metric)
 */

typedef struct Sol_s Sol;

struct Sol_s {
    my_m2 value;    /**< second path component */
};

typedef struct Dict_s Dict_t;


struct segment_list {
    short seg[10];
    char seg_type[10];
    int size;
};

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
};


typedef struct DictPath_s DictPath;

struct DictPath_s {
    Llist_t* paths[MAX_SIZE];
};


/**
 * @brief create a new empty Dictionary
 */

Dict_t* Dict_new();


/**
 * @brief allocate a Dictionary and set it to 0
 * 
 * @param dic           Dictionary to allocate
 * @param size          max size of the Dictionary
 * @param heapsize      size oh the dictionary's heaps
 * @param realSpread    real spread in the SR Graph
 */

void Dict_init(Dict_t* dic, int size);


/**
 * @brief unused for now
 */

void DictPath_init(DictPath* dic);


/**
 * @brief unused for now
 */

void DictPath_add(DictPath* dic, my_m1 key, Llist_t* path);


/**
 * @brief add a new path in a Dictionary
 * 
 * @param dic           Dictionary
 * @param key           first component of the path (delay)
 * @param value         second component of the path (IGP weight)
 * @param nbSeg         number of segment needed to build
 * this path
 */

void Dict_add(Dict_t* dic, my_m1 key, my_m2 value, short pred);


/**
 * @brief function used to reduce the Dictionary to a Pareto Front
 * 
 * @param dic           Dictionary to reduce
 * @param ext           Extendable paths
 * @param nbSeg         index of current step
 */

void Dict_reduce_to_pareto_front(Dict_t* dic, Extendable_t** ext, char nbSeg);


/**
 * @brief Used to reduce the Dictionary to a pareto front if 
 * there is a lot of new path at this step
 * 
 * @param dic           Dictionary to reduce
 * @param ext           Extendable paths
 * @param nbSeg         index of current step
 */

void Dict_reduce_to_pareto_front_all(Dict_t* dic, Extendable_t** ext, char nbSeg);


/**
 * @brief Used to reduce the Dictionary to a pareto front if 
 * there is not too many new path at this step
 * 
 * @param dic           Dictionary to reduce
 * @param ext           Extendable paths
 * @param nbSeg         index of current step
 */

void Dict_reduce_to_pareto_front_select(Dict_t* dic, Extendable_t** ext, char nbSeg);



/**
 * @brief Only use for debug
 */

void Dict_print(Dict_t* dic);


/**
 * @brief Only use for debug
 */

void Dict_print_stack(Dict_t* dic);


/**
 * @brief Use for crash test
 * 
 * This function emulate the worst pareto reduction possible
 */

void Dict_reduce_to_pareto_crash_test(Dict_t* dist, int nb_seg);


void Dict_free(Dict_t* dic);



#endif