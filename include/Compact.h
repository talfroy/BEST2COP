#ifndef __COMPACT_H__
#define __COMPACT_H__

#include "Dict.h"
#include "Pfront.h"

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
	path ***paths;
};

typedef struct compact_front compact_front;

path **compact_to_array_1D(Dict_t **dist, int *nb_paths, int iter, int nbNodes, struct segment_list ***sl);

int *get_nb_paths_per_dest(Dict_t **dist, int nbNodes, int iter);

void print_compact_array(path **compact_pf);
int **get_nb_paths_per_delay(Dict_t **dist, int nbNodes, int iter);

void print_compact_array_2D(compact_front *compact_pf);

compact_front* compact_to_array_2D(Pfront_t **pf, Dict_t **dist, int iter, int nbNodes, struct segment_list ***sl);

Dict_seglist_t **cart(compact_front *pf1, compact_front *pf2, int c1, int ABR);

struct segment_list merge_and_correct_sl(struct segment_list sl1, struct segment_list sl2);

extern void Compact_free(compact_front* cp);

extern Dict_t* compact_pareto_front_ify(Dict_seglist_t** merged[2], int nbNodes);


#endif