#include "../include/Best2cop.h"



int Best2cop(Pfront_t*** pfront, Dict_t** pf, SrGraph_t* graph, SrGraph_t* sr_conv, int src, my_m1 cstrM1, 
            my_m2 cstrM2, my_m1 dictSize, char analyse, int** iters, int bascule, int* init_time)
{
    (void) bascule;
    struct timeval start, stop;
    gettimeofday(&start, NULL);
    //Start of init
    // No SR : V-1 iteration at most
    int maxIter = graph->nbNode - 1;
    my_m2* minIgp = malloc(graph->nbNode * sizeof(my_m2));

    if (analyse) {
        maxIter = MAX(maxIter, SEG_MULT * SEG_MAX);
    }

    int nbIter = 1;

    // Two pfront because not-in-place
    (*pfront) = malloc((2) * sizeof(Pfront_t*));
    ASSERT(pfront, -1, (2));
    // One pfront per node
    for (int i = 0 ; i < 2 ; i++) {
        (*pfront)[i] = malloc(graph->nbNode * sizeof(Pfront_t));
        ASSERT((*pfront)[i], -1, graph->nbNode);

        for (int j = 0 ; j < graph->nbNode ; j++) {
            Pfront_init(&(*pfront)[i][j], dictSize, SEG_MAX);
        }
    }

    Dict_t* dist = malloc(graph->nbNode * sizeof(Dict_t));
    ASSERT(dist, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        Dict_init(&dist[i], dictSize, SEG_MAX);
    }

    // Init first distance
    Pfront_insert_key(&(*pfront)[0][src], 0, 0);
    Dict_add( &dist[src],0, 0, 0, src, NODE_SEGMENT, 0);

    // Create extendable of first distance
    Extendable_list_t** extendable = calloc(graph->nbNode, sizeof(Extendable_list_t*));

    ASSERT(extendable, -1, graph->nbNode);
    IntList_t* succs = graph->nonEmptySlots[src];
    int active_nodes[graph->nbNode];
    int active_nodes_nb = 0;
    // We only try to extend to v if the link u->v exists
    // All the successors of u will need to extend the (0,0) path
    while(succs != NULL)
    {
        int succ = succs->value;
        if (succ == src)
        {
            succs = succs->next;
            continue;
        }
        // Create a new extendable list for succ if new
        if (extendable[succ] == NULL) {
            extendable[succ] = Extendable_list_new(NULL, src, NULL);   
            active_nodes[active_nodes_nb] = succ;
            active_nodes_nb++;
        }
        // add the (0,0) path to the extendable list
        extendable[succ]->ext = Extendable_new(0, 0, 0, src, NODE_SEGMENT, extendable[succ]->ext);
     
        succs = succs->next;
    }

    Extendable_t** nextextendable = malloc(graph->nbNode * sizeof(Extendable_t*));
    ASSERT(nextextendable, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        nextextendable[i] = NULL;
    }

    (*pf) = malloc(graph->nbNode * sizeof(Dict_t));
    ASSERT((*pf), -1, graph->nbNode);
    for (int j = 0 ; j < graph->nbNode ; j++) {
        Dict_init((*pf) + j, dictSize, SEG_MAX);
    }
    

    Dict_add(&(*pf)[src], 0, 0, 0, src, NODE_SEGMENT, 0);

    if (iters != NULL) {
        for (int i = 0 ; i < graph->nbNode ; i++) {
            (*iters)[i] = -1;
            minIgp[i] = INF;
        }
        (*iters)[src] = 0;
    }
    bool to_extend = true;


    Dict_t pf_cand;
    Dict_init(&pf_cand, dictSize, SEG_MAX);

    gettimeofday(&stop, NULL);
    *init_time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

    while (to_extend && nbIter <= maxIter) {
       	#pragma omp parallel for schedule(dynamic)
        // go through all active nodes (that have to extend something)
        for (int idst = 0 ; idst < active_nodes_nb ; idst++) {
            int dst = active_nodes[idst];

            if (dst == src) {
                continue;
            }
            
            Dict_reset(&pf_cand);

            Pfront_t pfcandlist;
            Pfront_init(&pfcandlist, dictSize, SEG_MAX);
            int t = 0;
            my_m1 imax = 0;

            // extend the paths
            Best2cop_extend_path(dst, extendable[dst], &pf_cand, &pfcandlist, dist + dst, graph, sr_conv, &t, &imax, cstrM1, cstrM2);

            nextextendable[dst] = NULL;

            Best2cop_cpt_extendable_paths(nextextendable+dst, pfront, &pf_cand, dist + dst, &pfcandlist, t, imax, nbIter, dst, &(*pf)[dst], bascule, graph->nbNode);

            Pfront_free(&pfcandlist, SEG_MAX);
        }

        
        for (int j = 0; j < graph->nbNode; j++) {
            Extendable_list_free(extendable[j]);
            extendable[j] = NULL;
        }



        to_extend = false;
        int next_active_nodes_nb = 0;
        int next_active_nodes[graph->nbNode];
        IntList_t* succs;

        // Go through all active nodes that have have extended an ND paths to them
        for (int i = 0 ; i < active_nodes_nb ; i++) {
            if (nextextendable[active_nodes[i]] == NULL) {
                continue;
            }

            // go through the succ of the active node
            int node = active_nodes[i];
            succs = graph->nonEmptySlots[node];
            while(succs != NULL)
            {
                int succ = succs->value;
                if (succ == node)
                {
                    succs = succs->next;
                    continue;
                }

                // if new, create an extendable list for succ
                if (extendable[succ] == NULL) {
                    to_extend = true;
                    extendable[succ] = Extendable_list_new(NULL, node, Extendable_copy(nextextendable[node]));
                    next_active_nodes[next_active_nodes_nb] = succ;
                    next_active_nodes_nb++;

                } else {
                    extendable[succ] = Extendable_list_new(extendable[succ], node, Extendable_copy(nextextendable[node]));   
                }
                succs = succs->next;
            }
        } 
        active_nodes_nb = next_active_nodes_nb;
        for (int i = 0 ; i < next_active_nodes_nb ; i++) {
            active_nodes[i] = next_active_nodes[i];
        }

        
        for (int i = 0 ; i < graph->nbNode ; i++) {
            Extendable_free(nextextendable[i]);
            nextextendable[i] = NULL;
        }
        nbIter++;
    }
    
    gettimeofday(&start, NULL);


    Dict_free(&pf_cand);


    for (int j = 0 ; j < graph->nbNode ; j++) {
        Dict_free(&dist[j]);
        Extendable_list_free(extendable[j]);
    }
    free(extendable);
    free(dist);
    free(nextextendable);
    free(minIgp);
    gettimeofday(&stop, NULL);
    *init_time += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

    return 2;
}


my_m2 update_min_igp(my_m2 old, Extendable_t* nextext) {
    if (nextext == NULL) {
        return old;
    }

    return MIN(nextext->infos.m2, update_min_igp(old, nextext->next));
}

char isNotOnDAG(SrGraph_t *sr_conv, int currDag, int actNode, int dst, Edge_t *tmp)
{
    return ((sr_conv->m1dists[currDag][actNode] + tmp->m1 != sr_conv->m1dists[currDag][dst]) || (sr_conv->m2dists[currDag][actNode] + tmp->m2 != sr_conv->m2dists[currDag][dst]));
}

int addNecessarySegments(SrGraph_t *sr_conv, int *currDag, int actNode, int dst, Edge_t *tmp, int *d0v, int lastSegment)
{
    char edgeIsNotOnDag = isNotOnDAG(sr_conv, *currDag, actNode, dst, tmp);

    if ((*d0v) == 0 || edgeIsNotOnDag || lastSegment == ADJACENCY_SEGMENT)
    {
        (*d0v)++;
    }

    if (edgeIsNotOnDag)
    {
        *currDag = actNode;
        if (sr_conv->m1dists[actNode][dst] != tmp->m1 || sr_conv->m2dists[actNode][dst] != tmp->m2)
        {
            *currDag = dst;
            return ADJACENCY_SEGMENT;
        }
    }
    return NODE_SEGMENT;
}

void Best2cop_extend_path(int dst, Extendable_list_t* extendable, Dict_t* pf_cand, Pfront_t* pfcandlist,Dict_t* dist_v, SrGraph_t* graph, SrGraph_t* sr_conv, int* t, my_m1* imax, my_m1 c1, my_m2 c2)
{
    *imax = 0;
    *t = 0;
    for (Extendable_list_t* d_list = extendable ; d_list != NULL ; d_list = d_list->next) {
        short edgeSrc = d_list->node;
        for (Extendable_t* path = d_list->ext ; path != NULL ; path = path->next) {
            for (Edge_t* edge = graph->pred[dst][edgeSrc] ; edge != NULL ; edge = edge->next) {
                my_m1 d1v = path->infos.m1 + edge->m1;
                my_m2 d2v = path->infos.m2 + edge->m2;
                int d0v = path->infos.m0;
                int currDag = path->infos.currDag;
                int lastSegment = path->infos.lastSegment;
                // path->infos.

                lastSegment = addNecessarySegments(sr_conv, &currDag, edgeSrc, dst, edge, &d0v, lastSegment);
                           

                if (d1v < c1 && d2v < c2 && d0v < SEG_MAX) {     
                    if (d2v < dist_v->paths[d0v][d1v].m2)
                    {
                        Dict_add(dist_v, d0v, d1v, d2v, currDag, lastSegment, edgeSrc);
                        if (pf_cand->paths[d0v][d1v].m2 == INF) {
                            Pfront_insert_key(pfcandlist, d0v, d1v);
                            *t = *t + 1;
                        }
                        Dict_add(pf_cand, d0v, d1v, d2v, currDag, lastSegment, edgeSrc);
                        *imax = MAX(*imax, d1v);
                    }
                }              

            }
        }
    }
    // printf("End\n");

}


void Best2cop_cpt_extendable_paths(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, __attribute__ ((unused)) Pfront_t* pfcandlist, __attribute__ ((unused)) int t, 
                                    int imax, int iter, int dst, Dict_t* pf, int bascule, int nbNode)
{
    UNUSED(imax);
    UNUSED(pf);
    UNUSED(bascule);

    // if (t * log(t) + t + (*pfront)[iter-1][dst].heapSize < imax / 10) {
    //     if (dst == 8) {
    //         printf("On est dans select\n");
    //     }
    //     Best2cop_cpt_extendable_paths_select(nextextendable, pfront, pf_cand, dist_v, pfcandlist, iter, dst, pf);
    // } else {
    //     Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax, pf);
    // }

    //printf("%d\n", t);
    //printf("%d %d %d %d\n", dst , t, (*pfront)[iter-1][dst].heapSize, t + (*pfront)[iter-1][dst].heapSize);
    // printf("%d %d %d DIST\n", dst, t, iter);
    // printf("%d %d %d PFRONT\n", dst, (*pfront)[iter-1][dst].heapSize, iter);
    // printf("%d %d %d PFRONT+DIST\n", dst, (*pfront)[iter-1][dst].heapSize+t, iter);


    // if (bascule) {
    //     if (t  + (*pfront)[iter-1][dst].heapSize < bascule) {
    //         Best2cop_cpt_extendable_paths_select(nextextendable, pfront, pf_cand, dist_v, pfcandlist, iter, dst, pf);
    //     } else {
    //         Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax, pf);
    //     }
    // } else {
        Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax, pf, nbNode);
    // }
}


// void Best2cop_cpt_extendable_paths_select(Extendable_t** nextextendable, Pfront_t*** pfront,  Dict_t* pf_cand, Dict_t* dist_v, Pfront_t* pfcandlist, int iter, int dst, Dict_t* pf)
// {
//     // UNUSED(pf_cand);
//     // int pfrontIndex = 0;
//     // int pfcandIndex = 0;
//     // my_m2 last_d2 = INF;
//     // Pfront_t* pfront2 = &(*pfront)[iter-1][dst];
//     // Pfront_sort(pfcandlist);
//     // while (pfrontIndex < pfront2->heapSize && pfcandIndex < pfcandlist->heapSize) {
//     //     if (pfront2->keys[pfrontIndex] < pfcandlist->keys[pfcandIndex]) {
//     //         my_m1 d1 = pfront2->keys[pfrontIndex];
//     //         my_m2 d2 = dist_v->paths[d1];
//     //         if (d2 < last_d2) {
//     //             Pfront_insert_key(&(*pfront)[iter][dst], d1);
//     //             last_d2 = d2;
//     //         }
//     //         pfrontIndex++;
//     //     } else {
//     //         my_m1 d1 = pfcandlist->keys[pfcandIndex];
//     //         my_m2 d2 = dist_v->paths[d1];
//     //         if (d2 < last_d2) {
//     //             Pfront_insert_key(&(*pfront)[iter][dst], d1);
//     //             last_d2 = d2;
//     //            // if (pf_cand->paths[d1] != INF) {
//     //             Dict_add(pf, d1, d2, dist_v->preds[d1]);
//     //             (*nextextendable) = Extendable_new(d1, d2, (*nextextendable));
//     //             pf->nb_add++;
//     //            // }
//     //         }
//     //         pfcandIndex++;
//     //         if (pfront2->keys[pfrontIndex] == pfcandlist->keys[pfcandIndex]) {
//     //             pfrontIndex++;
//     //         }
//     //     }
//     // }

//     // while (pfrontIndex < pfront2->heapSize) {
//     //     my_m1 d1 = pfront2->keys[pfrontIndex];
//     //     my_m2 d2 = dist_v->paths[d1];
//     //     if (d2 < last_d2) {
//     //         Pfront_insert_key(&(*pfront)[iter][dst], d1);
//     //         last_d2 = d2;
//     //     }
//     //     pfrontIndex++;
//     // }

//     // while (pfcandIndex < pfcandlist->heapSize) {
//     //     my_m1 d1 = pfcandlist->keys[pfcandIndex];
//     //     my_m2 d2 = dist_v->paths[d1];
//     //     if (d2 < last_d2) {
//     //         Pfront_insert_key(&(*pfront)[iter][dst], d1);
//     //         last_d2 = d2;
//     //         Dict_add(pf, d1, d2, dist_v->preds[d1]);
//     //         (*nextextendable) = Extendable_new(d1, d2, (*nextextendable));
//     //         pf->nb_add++;
//     //     }
//     //     pfcandIndex++;
//     // }

//     Pfront_sort(pfcandlist);
//     Pfront_t* d1_it = Pfront_merge_sort(&(*pfront)[iter-1][dst], pfcandlist);
//     my_m2 last_d2 = INF;
//     for (int i = 0 ; i < d1_it->heapSize ; i++) {
//         my_m1 d1 = d1_it->keys[i];
//         my_m2 d2 = dist_v->paths[d1];
        
//         if (d2 < last_d2) {
//             Pfront_insert_key(&(*pfront)[iter][dst], d1);
//             last_d2 = d2;
            
//             if (pf_cand->paths[d1] != INF) {
//                 Dict_add(pf, d1, d2, dist_v->preds[d1]);
//                 (*nextextendable) = Extendable_new(d1, d2, (*nextextendable));
//                  pf->nb_add++;
//             }
//         }
//     }
//    Pfront_free(d1_it);
//    free(d1_it);
// }

void Best2cop_cpt_extendable_paths_all(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, int iter, int dst, int imax, Dict_t* pf, int nbNode)
{
    UNUSED(imax);
    UNUSED(iter);
    UNUSED(pfront);
    UNUSED(dst);

    // my_m1* sliding_best = malloc(sizeof(my_m1) * dist_v->max_m1);
    int ALL = nbNode;
    int min_currDag[nbNode+1];
    
    for (my_m0 i = 0 ; i < dist_v->max_m0 ; i++) {

        //init min_currDag to INF
        for (int i = 0 ; i < nbNode+1 ; i++) {
            min_currDag[i] = INF;
        }
        for (my_m1 j = 0 ; j < dist_v->max_m1 ; j++) {

            // update min_currDag for ALL
            if (i>0 && dist_v->paths[i-1][j].m2 < min_currDag[ALL]) {
                min_currDag[ALL] = dist_v->paths[i-1][j].m2;
            }

            // update the min_currDag 
            if (dist_v->paths[i][j].m2 < min_currDag[dist_v->paths[i][j].currDag]) {
                min_currDag[dist_v->paths[i][j].currDag] = dist_v->paths[i][j].m2;
            }

            
            if (dist_v->paths[i][j].m2 > min_currDag[ALL] 
            ||  dist_v->paths[i][j].m2 > min_currDag[dist_v->paths[i][j].currDag]) {
                dist_v->paths[i][j].m2 = MIN(
                    min_currDag[ALL],
                    min_currDag[dist_v->paths[i][j].currDag]
                );

                pf_cand->paths[i][j].m2 = INF; // not really needed
            } 
            else 
            {
                if (pf_cand->paths[i][j].m2 != INF) {
                    //Pfront_insert_key(&(*pfront)[iter%2][dst], i,j);
                    // printf("Adding %d %d %d from %d to %d\n", i, j, dist_v->paths[i][j].m2, dist_v->preds[i][j], dst);
                    Dict_add(pf, i,j, dist_v->paths[i][j].m2, dist_v->paths[i][j].currDag, dist_v->paths[i][j].lastSegment, 0);
                    pf->nb_add++;
                    (*nextextendable) = Extendable_new(i, j, pf_cand->paths[i][j].m2,dist_v->paths[i][j].currDag, dist_v->paths[i][j].lastSegment ,(*nextextendable));
                }

            }

        }
    }
    // printf("End2\n");
}
