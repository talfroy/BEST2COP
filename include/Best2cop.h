#ifndef __BEST2COP_H__
#define __BEST2COP_H__

#include "SrGraph.h"
#include "ParetoFront.h"
#include "Segment_list.h"


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

extern int Best2cop(Pfront_t*** pfront, Dict_t*** pf, SrGraph_t* graph, int src, my_m1 cstrM1, 
            my_m2 cstrM2, my_m1 dictSize, char full, int** iters);


/**
 * @brief Compute all the new paths using the extendable paths and the segment routing graph
 * 
 * @param dst               destination node of the extended paths
 * @param extendable        list of non-dominated paths found at 
 * last iteration
 * @param pf_cand           contains all the path added at the current step
 * @param pfcandlist        conatins all the index of the path added ath the
 * current step
 * @param dist_v            contains all the path toward node d
 * @param graph             segment routing graph
 * @param t                 by the end of the computing, will contain the number of
 * path added at the current step
 * @param imax              by the end of the computing, will contain the highest
 * index added ath the current step
 * @param c1                constraint on delay
 * @param c2                constraint on igp weight
 */

extern void Best2cop_extend_path(int dst, Extendable_list_t* extendable, Dict_t* pf_cand, Pfront_t* pfcandlist, 
                            Dict_t* dist_v, SrGraph_t* graph, int* t, my_m1* imax, my_m1 c1, my_m2 c2);


/**
 * @brief Call either select or all to extract the non
 * dominated paths
 * 
 * @param nextextendable    by the end of the computing, will contain
 * all the non-dominated paths toward dst found at the current iteration
 * @param pfront            will contain all the indexes of the non dominated
 * paths
 * @param pf_cand           contains all the paths added at the current step
 * @param pfcandlist        contains all the indexes of the paths added at 
 * the current step
 * @param t                 number of path toward dst added ath the current step
 * @param imax              index max
 * @param dst               destination node
 * @param pf                will contain all the non-domintaed paths
 */

extern void Best2cop_cpt_extendable_paths(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, Pfront_t* pfcandlist, int t, int imax, int iter, int dst, Dict_t* pf);


/**
 * @brief Called if there are not a lot of paths added
 * 
 * This function will reduce the pareto front selecting the 
 * indexes of the paths added at the current step
 */

extern void Best2cop_cpt_extendable_paths_select(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, Pfront_t* pfcandlist, int iter, int dst, Dict_t* pf);


/**
 * @brief Called if there are a lot of elements added ath the
 * current step
 * 
 * This function browse the whole pareto front, without checking
 * if there exists an element with a given index
 */

extern void Best2cop_cpt_extendable_paths_all(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, int iter, int dst, int imax, Dict_t* pf);


extern my_m2 update_min_igp(my_m2 old, Extendable_t* nextext);



#endif
