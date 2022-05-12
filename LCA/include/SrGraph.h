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




/**
 * @brief allocate all the structure
 * 
 * @param nbNodes           number of nodes in the Topology
 * 
 * @return return an allocate empty Segment Routing Graph structure
 */

extern SrGraph_t* SrGraph_init(int nbNodes);


/**
 * @brief create a full-mech graph corresponding to the 
 * given Topology using igp as the main metric
 * 
 * @param topo               Topology to transform onto a SR-graph
 * 
 * @return return the corresponding SR graph (at least full-mech)
 */

extern SrGraph_t* SrGraph_create_from_topology_best_m2(Topology_t* topo);


/**
 * @brief create a full-mech graph corresponding to the 
 * given Topology using delay as the main metric
 * 
 * @param topo               Topology to transform onto a SR-graph
 * 
 * @return return the corresponding SR graph (at least full-mech)
 */


extern SrGraph_t* SrGraph_create_from_topology_best_m1(Topology_t* topo);


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

extern LinkInfos* SrGraph_get_Edge_infos(SrGraph_t* graph, int src, int dst, int adjType);


/**
 * @brief free the SR Graph structure
 * 
 * @param graph             SR Graph structure
 */

extern void SrGraph_free(SrGraph_t* graph);


/**
 * @brief Only use for debug
 */

extern void SrGraph_print(SrGraph_t* graph, LabelTable_t* hash);


/**
 * @brief Transforme a Segment Routing Graph into a file
 * The file thus created can also been load via SrGraph_load_from_file
 * 
 * @param sr                SR Graph structure
 * @param output            output file
 */

extern void SrGraph_print_in_file(SrGraph_t* sr, FILE* output);

/**
 * @brief Save the SR Graph in the given file in binary
 * 
 * @param sr                SR Graph structure
 * @param filename          output filename
 */

extern void SrGraph_save_bin(SrGraph_t* sr, char* filename);


/**
 * @brief Load the SR Graph from the given file in binary
 * 
 * @param filename          input filename
 */

extern SrGraph_t* SrGraph_load_bin(char* filename);


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

extern SrGraph_t* SrGraph_load_with_id(char* filename, int nbNodes, int accuracy, char biDir);


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

extern SrGraph_t* SrGraph_load_with_label(char* filename, int accuracy, char biDir);


/**
 * @brief Create a crash test Topology
 * 
 * @param nbNode            number of node
 * @param nbPlinks          number of parallel links
 */

extern SrGraph_t* SrGraph_create_crash_test(int nbNode, int nbPlinks);


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

extern my_m1 SrGraph_get_max_spread(SrGraph_t* sr);


/**
 * @brief create a new graph representing the biggest connex component
 * of a SR graph
 * 
 * @param sr                Segment Routing graph to check
 * 
 * @return return the biggest connex component of sr
 */

extern SrGraph_t* SrGraph_get_biggest_connexe_coponent(SrGraph_t* sr);


/**
 * @brief create a flex algo from a topology. The flex algo SR graph
 * contains all the edges from the SR graph-best-m2 and from the 
 * SR graph-best-m1 (+ adjacencies)
 * 
 * @param topo              topology to transform
 * 
 * @return return the flex-algo sr graph
 */

extern SrGraph_t* SrGraph_create_flex_algo(Topology_t* topo);


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

extern SrGraph_t* SrGraph_merge_sr_graph(SrGraph_t* best_m1, SrGraph_t* best_m2, int size);


/**
 * @brief add all the adjacency segments from a topology to an 
 * sr graph
 * 
 * @param graph             SR graph
 * @param topo              topology
 * 
 * @return return the new sr graph
 */

extern SrGraph_t* SrGraph_add_adjacencies(SrGraph_t* graph, Topology_t* topo);


/**
 * @brief Print a Segment Routing Graph in a file, using the node label
 * for the links.
 * 
 * @param sr                Segment Routing Graph to print
 * @param output            Output stream
 * @param lt                label table of the topology used to 
 * compute the Segment Routing Graph.
 */

extern void SrGraph_print_in_file_labels(SrGraph_t* sr, FILE* output, LabelTable_t* lt);


#define PBSTR "############################################################"
#define PBWIDTH 60

extern void my_print_progress(int percentage);

#endif