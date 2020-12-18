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

Llist_t* Llist_new(Llist_t* next, my_m1 m1, my_m2 m2, int nodeId, int segType);


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

Llist_t* Llist_add(Llist_t* list, my_m1 m1, my_m2 m2, int nodeId, int segType);


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

Llist_t* Llist_remove(Llist_t* list, int nodeId);


/**
 * @brief give the length of a linked list
 * (the number of elements)
 * 
 * @param list          list to count
 * 
 * @return return list length
 */

int Llist_size(Llist_t* list);


/**
 * @brief free the memory allocate to a given linked list
 * 
 * @param list          list to free
 */

void Llist_free(Llist_t* list);


/**
 * @brief search an element in a linked list
 * 
 * @param list          list in which we search
 * @param nodeId        destination node for the link
 * @param adjType       Segment type
 * 
 * @return return the element if it is found
 */

Llist_t* Llist_search(Llist_t* list, int nodeId, int adjType);


/**
 * @brief compare all the element of two given 
 * linked lists
 * 
 * @param list1         first list
 * @param list2         second list
 * 
 * return true if the two lists are the same, false if not
 */

bool Llist_compare(Llist_t* list1, Llist_t* list2);


/**
 * @brief compare two informations blocs
 * 
 * @param LinkInfos1        first information bloc
 * @prama LinkInfos2        second information bloc
 * 
 * @return true if the two blocs are the same, false if not
 */

bool LinkInfos_compare(my_m1 m1, my_m2 m2, int nodeId, LinkInfos* LinkInfos2);

/**
 * @brief Only use for debug
 */

void Llist_print(Llist_t* list, FILE* file);

/**
 * @brief Only use for debug
 */

void Llist_print_label(Llist_t* list, FILE* file, LabelTable_t* hash);

/**
 * @brief Only use for debug
 */

void Llist_print_infos(Llist_t* list, FILE* file);


/**
 * @brief return the element of a linked list at a given position
 * 
 * @param list          list in which we search
 * @param pos           position of the searched element
 * 
 * @return return the element at position pos
 */

Llist_t* Llist_get_at_pos(Llist_t* list, int pos);


/**
 * @brief reverse a linked list
 * 
 * @param list          list to reverse
 * 
 * @return return the reversed list
 */

Llist_t* Llist_reverse(Llist_t* list);


/**
 * @brief update the corresponding link
 * 
 * 
 * @param list          list in which we search
 * @param key           delay
 * @param value         IGP weight
 * @param node          destination node
 * 
 * @return return the new linked list
 */

Llist_t* Llist_update(Llist_t* list, my_m1 m1, my_m2 m2, int node, int seg_type);


bool node_is_in_tab(int* node, int nbNodes, int nodeToTest);


Llist_t* Llist_prune(Llist_t* list, int* node, int nbNodes);


Llist_t* Llist_copy_select(Llist_t* list, int* node, int nbNodes);


Llist_t* Llist_copy(Llist_t* list);


#endif