#ifndef __COMPACT_H__
#define __COMPACT_H__


#include "Dict.h"


// struct compact_elt {
// 	int igp_cost;
// 	struct segment_list;
// };


// struct compact_front {
//     short* nb_delays; 
// 	short* nb_seg;
// 	compact_elt* compact_elts;
// };

struct path {
	my_m2 cost;
	my_m1 delay;
	struct segment_list sl;
};

// struct compact_paths{
// 	int igp_cost;
// 	int nb_seg;
// 	compact_paths* next;
// };

// struct compact_front {
// 	int delay; 
// 	compact_paths* paths;
// 	compact_front* next; 
// };

// typedef struct compact_front compact_front;
// typedef struct compact_paths compact_paths;
typedef struct path path;

// compact_front* compact_pfront(Dict_t** dist, int nbNodes);
path** compact_to_array(Dict_t** dist, int* nb_paths, int iter, int nbNodes, struct segment_list*** sl);
int* get_nb_paths_per_dest(Dict_t** dist, int nbNodes, int iter);
void print_compact_array(path** compact_pf);

#endif