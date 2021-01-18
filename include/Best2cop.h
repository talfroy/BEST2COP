#ifndef __BEST2COP_H__
#define __BEST2COP_H__

#include "SrGraph.h"
#include "ParetoFront.h"


/**
 * @brief ALGORITHME BEST2COP (Best Exact Segment Track for 2-Constrained Optimal Paths)
 * 
 * find all the availables and optimized paths from a source node to 
 * all the others nodes
 * 
 * @param dist              list of the distance between two nodes
 * this parameter must not be filled and not allocate
 * @param graph             SR graph to compute
 * @param src               source node
 * @param cstrM1            constraint on delay (the default value is 1000)
 * @param cstrM2            constraint on IGP value (the default value is MAX_INT)
 * @param dictSize          dictionary max size
 * @param realSpread        real delay spread on SR Graph
 * @param full              option used to check how much iterations
 * are needed to align the algorithme
 * @param iters             use to store the iterations value
 * (NULL if full is disable)
 * 
 * @return return the number of iteration (between 1 and SEGMAX)
 */

int Best2cop(Pfront_t*** pfront, ParetoFront_t**** pf, SrGraph_t* graph, int src, my_m1 cstrM1, my_m2 cstrM2, my_m1 dictSize, char full, __attribute__((unused)) int** iters);


void Best2cop_extend_path(int dst, Extendable_list_t* extendable, Dict_t* pf_cand, Pfront_t* pfcandlist, 
                            Dict_t* dist_v, SrGraph_t* graph, int* t, my_m1* imax, my_m1 c1, my_m2 c2);


void Best2cop_cpt_extendable_paths(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, Pfront_t* pfcandlist, int t, int imax, int iter, int dst, ParetoFront_t** pf);


void Best2cop_cpt_extendable_paths_select(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, Pfront_t* pfcandlist, int iter, int dst, ParetoFront_t** pf);


void Best2cop_cpt_extendable_paths_all(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, int iter, int dst, int imax, ParetoFront_t** pf);


my_m2 update_min_igp(my_m2 old, Extendable_t* nextext);

#endif