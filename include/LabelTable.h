#ifndef __LABELTABLE_H__
#define __LABELTABLE_H__


#include "base.h"

#define NB_NODE_ALLOC   100

/**
 * @brief structure which represent a node 
 */

typedef struct node_s node;

struct node_s {
    char name[128];
};

typedef struct LabelTable_s LabelTable_t;


/**
 * @brief structure which represent an hash table
 * 
 * all the nodes name are associated with an identifier
 */

struct LabelTable_s {
    node* node;
    int nextNodeId;
};


/**
 * @brief initialize the hash table
 */

void LabelTable_init(LabelTable_t* hash);


/**
 * @brief free the hash table
 */

void LabelTable_free(LabelTable_t* hash);


/**
 * @brief display the hash table
 */

void LabelTable_print(LabelTable_t* hash);


/**
 * @brief add a node with a name in the hash table
 * 
 * @param name          name of the node
 * 
 * @return return the ID of the node (if he already exist or not)
 */

int LabelTable_add_node(LabelTable_t* hash, char* name);


/**
 * @brief return the ID of a node with a given name
 * 
 * @param               name of the node
 * 
 * @return return the id if the node exists, -1 if not
 */

int LabelTable_get_id(LabelTable_t* hash, char* name);


/**
 * @brief return the node with a given ID
 * 
 * @param nodeId        ID of the given node
 * 
 * @return return the name of the node if he exists, NULL if not
 */

char* LabelTable_get_name(LabelTable_t* hash, int nodeId);


/**
 * @brief Sort the LabelTable using alphabetic order
 */

void LabelTable_sort(LabelTable_t* hash);

#endif