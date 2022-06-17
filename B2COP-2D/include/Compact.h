#ifndef __COMPACT_H__
#define __COMPACT_H__

#include "Dict.h"
#include "Pfront.h"
#include "SrGraph.h"
#include "Segment_list.h"

struct path
{
	my_m2 cost;
	my_m1 delay;
	struct segment_list sl;
};
typedef struct path path;

struct compact_front
{
	int nbNodes;
	int iter;
	int **nb_path;
	// first  dimension : nbNodes
	// second dimension : nbIter
	// third  dimension : nbPath
	path ***paths;
};

typedef struct compact_front compact_front;


/**
 * @brief unused for now
 */

extern path **compact_to_array_1D(Dict_t **dist, int *nb_paths, int iter, 
								int nbNodes, struct segment_list ***sl);


/**
 * @brief unused for now
 */

extern int *get_nb_paths_per_dest(Dict_t **dist, int nbNodes, int iter);

extern void print_compact_array(path **compact_pf);


/**
 * @brief returns a array containing for each node and each number of segments,
 * the number of paths.
 * 
 * @param dist				distance array to convert
 * @param nbNodes			first dimension size
 * @param iter				second dimension size
 * 
 * @return Return the array
 */

extern int **get_nb_paths_per_delay(Dict_t **dist, int nbNodes, int iter);


/**
 * @brief Only used for debug
 */

extern void print_compact_array_2D(compact_front *compact_pf);


/**
 * @brief Convert a distance pareto front to a Compact pareto front
 * A compact pareto front is an array of dimention 3 where 
 * - the first is the number of nodes
 * - the second is the number of segments
 * - the third is the number of path for each couple (nb nodes ; nb segments)
 * 
 * @param pf					Unused for now
 * @param dist					distance pareto front to convert
 * @param iter					number of max iterations
 * @param nbNodes				number of nodes in the topology (area)
 * @param sl					retreived segment list
 * 
 * @return Return the corresponding compact front
 */

extern compact_front* compact_to_array_2D(Pfront_t **pf, Dict_t **dist, int iter, 
						int nbNodes, struct segment_list ***sl);


/**
 * @brief Do the cartesian product between two compact front.
 * Correct the segment list if needed and return a dictonnary 
 * containing all the segment lists for each nodes, segment list
 * size and delay.
 * 
 * @param pf1					compact front of the first area
 * (area 0 the most of the time)
 * @param pf2					compact front of the other area
 * with the first ABR as source node
 * @param pf2bis				compact front of the other area
 * with the other ABR as source node
 * @param c1					constraint on delay
 * @param ABR					id of the first abr in the first area
 * @param other_ABR				id of the other abr in the first area
 * @param sr_bb					Segment Routing graph of the first area
 * (the backbone most of the time)
 * @param topo_bb				Topology of the first area
 * @param topo_area				Topology of the other area
 * @param src					source node of the first area
 * @param analyse				Used for eval
 * 
 * @return Return the corresponding dictionnary containing all the
 * segment lists.
 */

extern Dict_seglist_t **cart(compact_front *pf1, compact_front *pf2, compact_front *pf2bis, my_m1 c1, 
				int ABR, int other_ABR, SrGraph_t *sr_bb, Topology_t* topo_bb, Topology_t* topo_area, 
				int src, bool analyse);

extern void Compact_free(compact_front* cp);


/**
 * @brief Compact the cartesian products passing through the first abr
 * and the second. Also reduce it into a pareto front.
 * 
 * @param merged				Cartesian product result of all the distances
 * between all the two ABR. merged[0] represents the caretsian products of the
 * backbone with the second area with first ABR as source. merged[0] represents 
 * the caretsian products of the backbone with the second area with second ABR 
 * as source.
 * @param nbNodes				nbNodes in the second area
 * @param analyse				define the analyse type
 * 
 * @return Return the merged pareto front.
 */

extern Dict_seglist_t** compact_pareto_front_ify(Dict_seglist_t** merged[2], int nbNodes, bool analyse);


/**
 * @brief Convert a dictionnary of segment list into a compact front.
 * 
 * @param pf					unused
 * @param dist					dictionnary to convert
 * @param iter					Number max of iteration
 * @param nbNodes				Number of node in the corresponding topology
 * 
 * @return Return the corresponding compact front
 */

extern compact_front *dict_seglist_to_compact(Pfront_t **pf, Dict_seglist_t **dist, int iter, int nbNodes);


/**
 * @brief Quite same as compact_pareto_front_ify
 */

extern Dict_seglist_t **compact_pareto_front_ify_3D(Dict_seglist_t ***merged, int nbNodes, bool analyse);


#endif