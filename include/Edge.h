#ifndef __EDGE_H__
#define __EDGE_H__

#include "base.h"
#include "params.h"


typedef struct Edge_s Edge_t;

struct Edge_s {
    my_m1 m1;
    my_m2 m2;
    char id;
};

typedef struct Edge_tab_s Edge_tab_t;

struct Edge_tab_s {
    Edge_t* list;
    int size;
    int available;
};


/**
 * @brief allocate and fill a new edge structure
 * 
 * @param prev          next element in the list (old list)
 * @param m1            delay of the new edge
 * @param m2            igp weight of the new edge
 * 
 * @return return the new list
 */

void Edge_new(Edge_tab_t* prev, my_m1 m1, my_m2 m2);


void Edge_tab_init(Edge_tab_t* tab);


/**
 * @brief allocate and fill a new edge structure
 * 
 * This function allow us to force the id of the new element
 * 
 * @param prev          next element in the list (old list)
 * @param m1            delay of the new edge
 * @param m2            igp weight of the new edge
 * 
 * @return return the new list
 */

Edge_t* Edge_new_force_id(Edge_t* prev, my_m1 m1, my_m2 m2, char id);


/**
 * @brief allocate and fill a new edge structure
 * 
 * This function check if the new element already exists
 * 
 * @param prev          next element in the list (old list)
 * @param m1            delay of the new edge
 * @param m2            igp weight of the new edge
 * 
 * @return return the new list
 */

void Edge_add(Edge_tab_t* prev, my_m1 m1, my_m2 m2);


/**
 * @brief allocate a new edge list and copy the content of old
 * into the new list
 * 
 * @param old           edge list to copy
 * 
 * @return return the list's copy
 */

Edge_t* Edge_copy(Edge_t* old);

void Edge_free(Edge_t* edge);


/**
 * @brief merge two edge lists, maintaining the pareto front
 * 
 * @param best_m1       first list
 * @param best_m2       second list
 * 
 * @return return the merged list
 */

void Edge_merge_flex(Edge_tab_t* tab, Edge_t* best_m2);

void Edge_print_list(Edge_tab_t* list, FILE* output);


void Edge_tab_delete(Edge_tab_t* tab);


#define for_each_edge(prefix, edge, edge_tab)   \
    for (int prefix ## _iter = 0 ; prefix ## _iter < edge_tab.size && (edge = edge_tab.list[prefix ## _iter], 1) ; prefix ## _iter++)

#endif