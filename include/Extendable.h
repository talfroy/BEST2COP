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
    Path* infos;
    size_t available;
    size_t count;
};


#define foreach_extendable(p, extendable_array) \
    for(const Path *p = &((extendable_array)->infos[0]); \
        p - (extendable_array)->infos < (long)(extendable_array)->count ; \
        p++)


#define foreach_extendable_list(l, extendable_list_array) \
    for(const Extendable_list_element_t *l = &((extendable_list_array)->node_ext[0]); \
        l - (extendable_list_array)->node_ext < (long)(extendable_list_array)->count ; \
        l++)

typedef struct {
    short        node;
    Extendable_t ext;
} Extendable_list_element_t;

typedef struct Extendable_list_s Extendable_list_t;

struct Extendable_list_s {
    Extendable_list_element_t * node_ext;
    
    size_t available;
    size_t count;
};


/**
 * @brief add a new Extendable Path
 * 
 * @param m1                delay of the new Path
 * @param m2                IGP weight of the new Path
 * @param next              old list of Extendables Paths
 * 
 * @return return the new list
 */

Extendable_t* Extendable_create(void);

void Extendable_append(Extendable_t* ext, my_m1 m1, my_m2 m2);

void Extendable_clear(Extendable_t* ext);


void Extendable_free(Extendable_t* ext);


/**
 * @brief only use for debug
 */

void Extendable_print(Extendable_t* ext);

/**
 * @brief check if all the element of an array of ext are NULL
 * 
 * @param ext               array to test
 * @param nbNode            array size
 * 
 * @return return true if the array is empty, false if not
 */

bool Extendable_is_empty(Extendable_t** ext, int nbNode);



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

void Extendable_list_append(Extendable_list_t* extl, int node, const Extendable_t ext);

int Extendable_list_is_empty(const Extendable_list_t* extl);

void Extendable_list_clear(Extendable_list_t* extl);



void Extendable_list_free(Extendable_list_t* extl);

#endif