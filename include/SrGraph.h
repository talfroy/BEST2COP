#ifndef __SRGRAPH_H__
#define __SRGRAPH_H__

#include "base.h"
#include "Topology.h"
#include "Llist.h"
#include "Dict.h"
#include "LabelTable.h"
#include "Edge.h"
#include "Pfront.h"

typedef struct SrGraph_s SrGraph_t;

/**
 * @brief This structure represent a Segment Routing Graph
 * 
 * In this structure, an Edge between two node represent the 
 * shortest path between these two node on the original 
 * Topology
 */

struct SrGraph_s {
    my_m1** m1dists;        /**< This member represent the delays between the nodes */
    my_m2** m2dists;        /**< This member represent the IGP weight between the nodes */
    Edge_t*** pred;           /**< List of the predecessors */
    //Edge_t*** succ;           /**< List of the successors */
    int nbNode;             /**< exact number of node */
};

typedef struct edge_s edge;

struct edge_s {
    int src;
    int dst;
};




/**
 * @brief allocate all the structure
 * 
 * @param nbNodes           number of nodes in the Topology
 * 
 * @return return an allocate empty Segment Routing Graph structure
 */

SrGraph_t* SrGraph_init(int nbNodes);


/**
 * @brief create a full-mech graph corresponding to the 
 * given Topology using igp as the main metric
 * 
 * @param topo               Topology to transform onto a SR-graph
 * 
 * @return return the corresponding SR graph (at least full-mech)
 */

SrGraph_t* SrGraph_create_from_topology_best_m2(Topology_t* topo);


/**
 * @brief create a full-mech graph corresponding to the 
 * given Topology using delay as the main metric
 * 
 * @param topo               Topology to transform onto a SR-graph
 * 
 * @return return the corresponding SR graph (at least full-mech)
 */


SrGraph_t* SrGraph_create_from_topology_best_m1(Topology_t* topo);


/**
 * @brief give the information of a given link
 * 
 * @param graph             SR graph in which we search the informations
 * @param src               source node
 * @param dst               destination node
 * 
 * return return the information of the link (src ; dst) if it exists
 * NULL if not
 */

LinkInfos* SrGraph_get_Edge_infos(SrGraph_t* graph, int src, int dst, int adjType);


/**
 * @brief free the SR Graph structure
 * 
 * @param graph             SR Graph structure
 */

void SrGraph_free(SrGraph_t* graph);


/**
 * @brief Only use for debug
 */

void SrGraph_print(SrGraph_t* graph, LabelTable_t* hash);


/**
 * @brief Transforme a Segment Routing Graph into a file
 * The file thus created can also been load via SrGraph_load_from_file
 * 
 * @param sr                SR Graph structure
 * @param output            output file
 */

void SrGraph_print_in_file(SrGraph_t* sr, FILE* output);

/**
 * @brief Save the SR Graph in the given file in binary
 * 
 * @param sr                SR Graph structure
 * @param filename          output filename
 */

void SrGraph_save_bin(SrGraph_t* sr, char* filename);

/**
 * @brief Load the SR Graph from the given file in binary
 * 
 * @param filename          input filename
 */
SrGraph_t* SrGraph_load_bin(char* filename);

/**
 * @brief remove all the Edge which violate at least one constraint
 * 
 * @param graph             SR graph filled by a Topology
 * @param src               source from which we want a path
 * @param cstrIgp           constraint on first component
 * @param cstrDelay         constraint on second component
 */

void SrGraph_prune(SrGraph_t* graph, int src, my_m1 cstrM1, my_m2 cstrM2);



/**
 * @brief use the result of bellman fork to give a Segment sequence
 * 
 * @param graph             SR graph to compute
 * @param paths             all the paths from src to all destination
 * nodes which respects the constriants
 * this parameter must not be filled and not allocate
 * @param preds             result of bellman Fork
 * @param src               source node
 */

void SrGraph_retrieve_path(SrGraph_t* graph, Dict_t*** paths, Dict_t** preds, int src);


/**
 * @brief load a Segment Routing from a file
 * 
 * This function is used to load file in which node
 * are represented by ids
 * 
 * @param filename          file to load
 * @param nbNodes           number of node in the SR Graph to load
 * (MAX_SR_GRAPH_SIZE by default)
 * @param accuracy          delay's trueness
 * @param biDir             define if edges are bi-directionals or not
 */

SrGraph_t* SrGraph_load_with_id(char* filename, int nbNodes, int accuracy, char biDir);


/**
 * @brief load a Segment Routing from a file
 * 
 * This function is used to load file in which node
 * are represented by labels
 * 
 * @param filename          file to load
 * @param accuracy          delay's trueness
 * @param biDir             define if edges are bi-directionals or not
 */

SrGraph_t* SrGraph_load_with_label(char* filename, int accuracy, char biDir);


/**
 * @brief Create a crash test Topology
 * 
 * @param nbNode            number of node
 * @param nbPlinks          number of parallel links
 */

SrGraph_t* SrGraph_create_crash_test(int nbNode, int nbPlinks);


SrGraph_t* SrGraph_create_diablo(int nbNode, int nbPlinks, char mode);


/**
 * @brief Return the max delay in a SR Graph
 * 
 * Can also detect if the base topology is a connexe 
 * component or not
 * 
 * @param sr                SR Graph
 * 
 * @return return the max delay value, or -1 if the topology
 * is not a conexe component
 */

my_m1 SrGraph_get_max_spread(SrGraph_t* sr);


/**
 * @brief Create a double full-mesh random SR Graph
 * 
 * The delay are choosen between 1 and maxSpread
 * The IGP weight are choosen between 1 and MAX_INT / SEGMAX
 * 
 * @param nbNode            number of node in the random SR Graph
 * @param maxSpread         delays' max spread
 */

SrGraph_t* SrGraph_create_random_topo(int nbNode, int maxSpread);


/**
 * @brief create a new graph representing the biggest connex component
 * of a SR graph
 * 
 * @param sr                Segment Routing graph to check
 * 
 * @return return the biggest connex component of sr
 */

SrGraph_t* SrGraph_get_biggest_connexe_coponent(SrGraph_t* sr);


/**
 * @brief create a flex algo from a topology. The flex algo SR graph
 * contains all the edges from the SR graph-best-m2 and from the 
 * SR graph-best-m1 (+ adjacencies)
 * 
 * @param topo              topology to transform
 * 
 * @return return the flex-algo sr graph
 */

SrGraph_t* SrGraph_create_flex_algo(Topology_t* topo);


/**
 * @brief merge all the edges (and reduce to pareto front) from two 
 * SR graphs
 * 
 * @param best_m1           first sr graph
 * @param best_m2           second sr graph
 * @param size              number of node in the sr graph
 * 
 * @return return th merged sr graph
 */

SrGraph_t* SrGraph_merge_sr_graph(SrGraph_t* best_m1, SrGraph_t* best_m2, int size);


/**
 * @brief add all the adjacency segments from a topology to an 
 * sr graph
 * 
 * @param graph             SR graph
 * @param topo              topology
 * 
 * @return return the new sr graph
 */

SrGraph_t* SrGraph_add_adjacencies(SrGraph_t* graph, Topology_t* topo);


void SrGraph_check_m1(SrGraph_t*);


/**
 * @brief check if a sr graph is connex
 */

bool SrGraph_is_connex(SrGraph_t* sr);


extern void SrGraph_print_in_file_labels(SrGraph_t* sr, FILE* output, LabelTable_t* lt);


#define PBSTR "############################################################"
#define PBWIDTH 60

extern void my_print_progress(int percentage);

#endif