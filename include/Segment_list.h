#ifndef __SEGMENT_LIST_H__
#define __SEGMENT_LIST_H__

#include "base.h"
#include "Topology.h"
#include "SrGraph.h"

#define SSTACK_SIZE         100

/**
 * @brief Structure that represents a list of segments 
 * Store in order the segments ID and the Segment types
 */

struct segment_list {
    short seg[SSTACK_SIZE];
    char seg_type[SSTACK_SIZE];
    int size;
    int abr_index;
};

typedef struct segment_list Segment_list_t;


/**
 * @brief Dictionnary used to store segment lists.
 * The segment list at index i is the segment list representing the
 * distance with delay of i
 */

struct Dict_seg_list_s {
    my_m2* paths;
    Segment_list_t* seg_list;
    int size;
};

typedef struct Dict_seg_list_s Dict_seglist_t;


/**
 * @brief Initialize a dictionnary of segment list
 * 
 * @param dic               dictionnary to init
 * @param size              size of the paths structure
 */

extern void Dict_seglist_init(Dict_seglist_t* dic, int size);


/**
 * @brief Add a segment list to the dictionnary on the right index
 * 
 * @param dic               dictionnary to complete
 * @param key               index of the segment list to add
 * @param value             IGP cost of the segment list
 * @param seglist           segment list to add in dic
 */

extern void Dict_seglist_add(Dict_seglist_t* dic, my_m1 key, my_m2 value, struct segment_list seglist);

extern void Dict_seglist_free(Dict_seglist_t* dic);


/**
 * @brief use for debug
 */

extern void Segment_list_print(FILE* stream, struct segment_list* sl, Topology_t* topo1, Topology_t* topo2);


/**
 * @brief use for debug
 */

extern void Segment_list_print_id(FILE* stream, struct segment_list *sl);



extern void segment_list_free(struct segment_list*** sl, int maxiter, int nbNodes);

extern struct segment_list*** Segment_list_retreive_paths(Dict_t** dist, SrGraph_t* sr, int maxiter, short src);

extern void print_segment_list(struct segment_list*** sl, int maxiter, int nbNodes);

extern void segment_list_invert(struct segment_list *sl);

extern void Segment_list_print_analyse(FILE *stream, Dict_seglist_t **final, int nbNodes, int nbIters, char analyse, Topology_t* topo_area);

extern void Dict_sl_print(Dict_seglist_t* dic);

#endif