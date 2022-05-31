#ifndef __LLIST_H__
#define __LLIST_H__

#include "base.h"
#include "LabelTable.h"

typedef struct LinkInfos_s LinkInfos;


/**
 * @brief Contains all the links informations
 */

struct LinkInfos_s {
    int edgeDst;         /**< destination node id */
    my_m1 m1;           /**< delay of the link */
    my_m2 m2;           /**< IGP weight of the link */
    int segType;        /**< segment type (adjacency or node) */
};

typedef struct Llist_s Llist_t;

/**
 * @brief strcuture of a link
 */

struct Llist_s {
    LinkInfos infos;
    Llist_t* next;
};



/**
 * @brief create a new element of a linked list
 * 
 * @param next          list before adding the new element
 * @param m1            link's delay
 * @param m2            link's IGP weight
 * @param edgeDst       link's destination node
 * @param segType       segment type (only use for SR Graphs)
 * 
 * @return return the new list after the add
 */

extern Llist_t* Llist_new(Llist_t* next, my_m1 m1, my_m2 m2, int nodeId, int segType);


/**
 * @brief create a new element of a linked list
 * if the element doesn't exist in the given list
 * 
 * In this function there is a redundancy test : you cannot
 * add the same link twice
 * 
 * @param list          list befor the add
 * @param m1            link's delay
 * @param m2            link's IGP weight
 * @param edgeDst       link's destination node
 * @param segType       segment type (only use for SR Graphs)
 * 
 * @return return the new list
 */

extern Llist_t* Llist_add(Llist_t* list, my_m1 m1, my_m2 m2, int nodeId, int segType);


/**
 * @brief remove the element with nodeId in his
 * informations if it exists
 * 
 * @param list          list in which we remove
 * @param edgeDst       the edge with edgeDst as destination
 * need to be removed
 * 
 * @return return the new list
 */

extern Llist_t* Llist_remove(Llist_t* list, int nodeId);


/**
 * @brief give the length of a linked list
 * (the number of elements)
 * 
 * @param list          list to count
 * 
 * @return return list length
 */

extern int Llist_size(Llist_t* list);


/**
 * @brief free the memory allocate to a given linked list
 * 
 * @param list          list to free
 */

extern void Llist_free(Llist_t* list);


/**
 * @brief search an element in a linked list
 * 
 * @param list          list in which we search
 * @param nodeId        destination node for the link
 * @param adjType       Segment type
 * 
 * @return return the element if it is found
 */

extern Llist_t* Llist_search(Llist_t* list, int nodeId, int adjType);


/**
 * @brief compare all the element of two given 
 * linked lists
 * 
 * @param list1         first list
 * @param list2         second list
 * 
 * return true if the two lists are the same, false if not
 */

extern bool Llist_compare(Llist_t* list1, Llist_t* list2);


/**
 * @brief Only use for debug
 */

extern void Llist_print(Llist_t* list, FILE* file);

/**
 * @brief Only use for debug
 */

extern void Llist_print_label(Llist_t* list, FILE* file, LabelTable_t* hash);

/**
 * @brief Only use for debug
 */

extern void Llist_print_infos(Llist_t* list, FILE* file);


/**
 * @brief return the element of a linked list at a given position
 * 
 * @param list          list in which we search
 * @param pos           position of the searched element
 * 
 * @return return the element at position pos
 */

extern Llist_t* Llist_get_at_pos(Llist_t* list, int pos);


#endif