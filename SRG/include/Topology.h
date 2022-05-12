#ifndef __TOPOLOGY_H__
#define __TOPOLOGY_H__

#include "base.h"
#include "Llist.h"
#include "LabelTable.h"
#include "ScopeStack.h"
#include "Dict.h"
#include "Edge.h"
#include "BinHeap.h"
#include "Zipf.h"

#define NB_NODE_MAX    1500

typedef struct Topology_s Topology_t;

/**
 * @brief structure representing a valuate graph
 */

struct Topology_s {
    int nbNode;             /**< number of node in a Topology */
    Llist_t** pred;           /**< list of all the predessessor for each node */
    Llist_t** succ;           /**< list of all the successor for each node */
    LabelTable_t* labels;
};


/**
 * @brief init an empty but allocate Topology with nbNode nodes
 * 
 * @param nbNodes           number of node in the new topology
 * 
 * @return return the empty Topology
 */

extern Topology_t* Topology_init(int nbNode);


/**
 * @brief allocate and load a Topology from a ISP graph file
 * 
 * @param filename          file name
 * @param accuracy          delay's trueness (1 means the accuracy is about 0.1ms)
 * @param biDir             define if the links are bi-directionals or not
 * 
 * @return return the corresponding Topology
 */

extern Topology_t* Topology_load_from_file_labels(const char* filename, int precision, char bi_dir);
extern Topology_t* Topology_load_from_file_ids(const char* filename, int precision, char bi_dir);


/**
 * @brief Load a topology and store it in multiple areas.
 * 
 * @param filename          Topology file to load
 * @param precision         Delay accuracy
 * @param bi_dir            Tells if the links are bi-directionnal
 * or not
 * @param nb_areas          Number of areas in the loaded topology
 * 
 * @return Returns a topology array, where topo[i] is area i
 */

extern Topology_t** Topology_load_multiple_areas(const char* filename, int precision, char bi_dir, int nb_areas);


/**
 * @brief return the metrics of a given link
 * 
 * @param topo              Topology in which we want to search
 * @param src               source node
 * @param dst               destination node
 * 
 * @return return the infos if the link exists, NULL if not
 */

extern LinkInfos* Topology_get_edge_infos(Topology_t* topo, int src, int dst, int adjType);


extern void Topology_print(Topology_t* topo, char* topoF);


/**
 * @brief free the Topology
 * 
 * @param topo              Topology to freed
 */

extern void Topology_free(Topology_t* topo);


/**
 * @brief implementation of the dijkstra algorithm
 * 
 * This function is used to do the All Pairs Shortest Path
 * needed for the Segment Routing Graph transformation
 * this function takes the igp weight as main metric
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

extern void dikjstra_best_m2(Edge_t**** predOutGraph, Llist_t** ingraph, 
                int root, my_m1** m1dists, my_m2** m2dists, int nbNodes);


/**
 * @brief implementation of the dijkstra algorithm
 * 
 * This function is used to do the All Pairs Shortest Path
 * needed for the Segment Routing Graph transformation 
 * this funtion takes the delay as the main metric
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

extern void dikjstra_best_m1(Edge_t**** predOutGraph, Llist_t** ingraph, 
                int root, my_m1** m1dists, my_m2** m2dists, int nbNodes);


extern long int my_pow(long int x, int y);


/**
 * @brief create a full-mesh topology with weights following
 * a zipf distribution
 * 
 * @param size              number of nodes in the topology
 * @param v_delay           array containing the delays values following
 * the zipf distribution
 * @param v_igp             array containing the igp weights values following
 * the zipf distribution
 * 
 * @return return the random topology
 */

extern Topology_t* Topology_create_random(int size, int v_delay[], int v_igp[]);


/**
 * @brief create a topology weighted following a zipf distribution
 * Each pair of node (i ; j) with j > i has a probability
 * of exist/size to have a bidirectional link
 * 
 * @param size              number of nodes in the topology
 * @param v_delay           array containing the delays values following
 * the zipf distribution
 * @param v_igp             array containing the igp weights values following
 * the zipf distribution
 * @param exist             existence coefficient
 * 
 * @return return the random topology
 */

extern Topology_t* Topology_create_random_quentin(int size, int v_delay[], int v_igp[], int exist);


/**
 * @brief Get the id of th required ABR (note that the name of the node must
 * be on the good format)
 * 
 * @param topo              Area in which we search
 * @param area1             Right ABR area
 * @param area2             Left  ABR area
 * @param id                ABR id (0 or 1)
 * 
 * @return Returns the id of the required ABR if it exists, -1 if not
 */

extern int Topology_search_abr_id(Topology_t* topo, int area1, int area2, int id);


/**
 * @brief Get the id of th required ABR (note that the name of the node must
 * be on the good format)
 * 
 * @param topo              Area in which we search
 * @param area              ACC area number
 * @param aggGroup          Aggregation Group of the acces router
 * @param id                Acces router ID
 * 
 * @return Returns the id of the required ABR if it exists, -1 if not
 */

extern int Topology_search_acc_id(Topology_t* topo, int area, int aggGroup, int id);


#endif