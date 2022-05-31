#ifndef __CRASH_TEST_H__
#define __CRASH_TEST_H__


#include "SrGraph.h"
#include "base.h"
#include "params.h"



/**
 * @brief Crash Test
 * 
 * This function is only a crash test, used to push BESTMCP aside
 * Here we don't use a real Topology but only a "phantom" Topology.
 * It means the values doesn't matter, but only the amount of links
 * It emulate the algorithme like if each new add is a new optimized path
 * 
 * @param graph             Segment Routing Graph used
 * @param dist              Output, the structure with all the
 * optimized paths
 * @param nb_para           Number of parallel links
 * 
 * @return return the number of iteration (number of add_Dict calls)
 */

extern long int CrashTest(SrGraph_t* graph, my_m1 cstrM1, int nbPlLinks, int realSpread);

extern void CrashTest_extend_path(SrGraph_t* graph, Extendable_list_t* extendable, Dict_t* dist_v, Dict_t* pf_cand, Pfront_t* pfcandlist, int dst);

extern void CrashTest_cpt_extendable_paths(Extendable_t** nextextendable, Pfront_t*** pfront, int realSpread, Dict_t* dist_v, 
                                    int dst, int nbPara, int nbNode, int currStep, int cstrM1);

#endif