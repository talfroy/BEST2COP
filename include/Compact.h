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

path **compact_to_array_1D(Dict_t **dist, int *nb_paths, int iter, int nbNodes, struct segment_list ***sl);


/**
 * @brief unused for now
 */

int *get_nb_paths_per_dest(Dict_t **dist, int nbNodes, int iter);

void print_compact_array(path **compact_pf);


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

int **get_nb_paths_per_delay(Dict_t **dist, int nbNodes, int iter);


/**
 * @brief 
 */

void print_compact_array_2D(compact_front *compact_pf);

compact_front* compact_to_array_2D(Pfront_t **pf, Dict_t **dist, int iter, int nbNodes, struct segment_list ***sl);

Dict_seglist_t **cart(compact_front *pf1, compact_front *pf2, compact_front *pf2bis, int c1, int ABR, int other_ABR, SrGraph_t *sr_bb, Topology_t* topo_bb, Topology_t* topo_area, int src);

struct segment_list merge_and_correct_sl(struct segment_list sl1, struct segment_list sl2, compact_front *pf_area_abr1, compact_front *pf_area_abr2, SrGraph_t *sr_bb, int other_abr, Topology_t* topo_bb, Topology_t* topo_area, int src);

extern void Compact_free(compact_front* cp);

extern Dict_seglist_t** compact_pareto_front_ify(Dict_seglist_t** merged[2], int nbNodes);


#endif