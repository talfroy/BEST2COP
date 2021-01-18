#ifndef __TOPOLOGY_H__
#define __TOPOLOGY_H__

#include "base.h"
#include "Llist.h"
#include "LabelTable.h"
#include "ScopeStack.h"
#include "Dict.h"
#include "Edge.h"
#include "BinHeap.h"

#define NB_NODE_MAX    1500

typedef struct Topology_s Topology_t;

/**
 * @brief structure representing a valuate graph
 */

struct Topology_s {
    int nbNode;             /**< number of node in a Topology */
    Llist_t** pred;           /**< list of all the predessessor for each node */
    Llist_t** succ;           /**< list of all the successor for each node */
};


/**
 * @brief init an empty but allocate Topology with nbNode nodes
 * 
 * @param nbNodes           number of node in the new topology
 * 
 * @return return the empty Topology
 */

Topology_t* Topology_init(int nbNode);


/**
 * @brief allocate and load a Topology from a ISP graph file
 * 
 * @param filename          file name
 * @param accuracy          delay's trueness (1 means the accuracy is about 0.1ms)
 * @param biDir             define if the links are bi-directionals or not
 * 
 * @return return the corresponding Topology
 */

Topology_t* Topology_load_from_file(const char* filename, int precision, char bi_dir);


/**
 * @brief return the metrics of a given link
 * 
 * @param topo              Topology in which we want to search
 * @param src               source node
 * @param dst               destination node
 * 
 * @return return the infos if the link exists, NULL if not
 */

LinkInfos* Topology_get_edge_infos(Topology_t* topo, int src, int dst, int adjType);


void Topology_print(Topology_t* topo);


/**
 * @brief free the Topology
 * 
 * @param topo              Topology to freed
 */

void Topology_free(Topology_t* topo);


/**
 * @brief implementation of the dijkstra algorithm
 * 
 * This function is used to do the All Pairs Shortest Path
 * needed for the Segment Routing Graph transformation
 * 
 * Here typycally, we just allocate an empty SR graph, and use
 * it arguments to call this function (succOutGraph, predOutGraph,
 * igpWeight and delays)
 * 
 * @param succOutGraph      pointer on the output graph successor array
 * this parameter must be allocated but not filled
 * @param predOutGraph      pointer on the output graph predecessor array
 * this parameter must be allocated but not filled
 * @param ingraph           input graph (typycally topo->succ)
 * @param root              root node to start the algorithm
 * @param m1dists           array which contains the delays from root
 * to the other nodes
 * this parameter must be allocated but not filled
 * @param m2dists           array which contains the IGP weights from root
 * to the other nodes
 * this parameter must be allocated but not filled
 * @param nbNodes           number of nodes in the Topology
 **/

void dikjstra_best_m2(Edge_t**** succOutGraph, Edge_t**** predOutGraph, Llist_t** ingraph, 
                int root, my_m1** m1dists, my_m2** m2dists, int nbNodes);


void dikjstra_best_m1(Edge_t**** succOutGraph, Edge_t**** predOutGraph, Llist_t** ingraph, 
                int root, my_m1** m1dists, my_m2** m2dists, int nbNodes);


long int my_pow(long int x, int y);

Topology_t* Topology_create_random(int size, my_m2 max_m2);

#endif