#ifndef __EXTENDABLE_H__
#define __EXTENDABLE_H__

#include "base.h"
#include "LabelTable.h"

typedef struct Path_s Path;


/**
 * @brief informations of an Extendable Path
 */

struct Path_s {
    my_m1 m1;       /**< represent the delay of the corresponding path */
    my_m2 m2;       /**< represent the IGP weight of the corresponding path */
};

typedef struct Extendable_s Extendable_t;

/**
 * @brief Structure which represents all the 
 * Extendables Paths
 */

struct Extendable_s {
    Path *infos;

    size_t allocated;
    size_t size;
};


typedef struct Extendable_list_s Extendable_list_t;

/**
 * @brief structure representing a linked list of Extendable paths
 */

struct Extendable_list_s {
    int node;
    //array of extendable
    Extendable_t ** ext;
    //array of extendable source
    int * sources;

    //allocated size
    size_t allocated;
    size_t size;
};

Extendable_t* Extendable_create(void);

/**
 * @brief add a new Extendable Path
 * 
 * @param m1                delay of the new Path
 * @param m2                IGP weight of the new Path
 * @param next              old list of Extendables Paths
 * 
 * @return return the new list
 */

void Extendable_add(Extendable_t* next, my_m1 m1, my_m2 m2);


void Extendable_clear(Extendable_t* next);

extern void Extendable_free(Extendable_t* ext);


/**
 * @brief only use for debug
 */

extern void Extendable_print(Extendable_t* ext);

/**
 * @brief check if all the element of an array of ext are NULL
 * 
 * @param ext               array to test
 * @param nbNode            array size
 * 
 * @return return true if the array is empty, false if not
 */

extern bool Extendable_is_empty(Extendable_t** ext, int nbNode);

/**
 * @brief create a new element for a list of Extendable_list
 * 
 * @param next              old list
 * @param node              destination node of the paths
 * @param ext               all the path toward node
 * 
 * @return return the new list
 */

Extendable_list_t* Extendable_list_create(void);

void Extendable_list_add(Extendable_list_t* next, int node, Extendable_t* ext);

void Extendable_list_free(Extendable_list_t* next);
void Extendable_list_clear(Extendable_list_t* next);

size_t Extendable_list_size(Extendable_list_t* next);
#endif