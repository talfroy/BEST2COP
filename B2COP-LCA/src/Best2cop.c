#include "../include/Best2cop.h"



int Best2cop(Pfront_t*** pfront, Dict_t*** pf, Topology_t* graph, SrGraph_t* sr, int src, my_m1 cstrM1, 
            my_m2 cstrM2, my_m1 dictSize, char analyse, int** iters)
{
    //Start of init
    // printf("T\n");

    UNUSED(pfront);
    int maxIter = graph->nbNode-1;
    my_m2* minIgp = malloc(graph->nbNode * sizeof(my_m2));

    if (analyse) {
        maxIter = MAX(maxIter, SEG_MULT * SEG_MAX);
    }

    int nbIter = 1;

    (*pfront) = malloc((maxIter + 1) * sizeof(Pfront_t*));
    ASSERT(pfront, -1, (maxIter + 1));

    // for (int i = 0 ; i <= maxIter ; i++) {
    //     (*pfront)[i] = malloc(graph->nbNode * sizeof(Pfront_t));
    //     ASSERT((*pfront)[i], -1, graph->nbNode);

    //     for (int j = 0 ; j < graph->nbNode ; j++) {
    //         Pfront_init(&(*pfront)[i][j], dictSize);
    //     }
    // }

    // Pfront_insert_key(&(*pfront)[0][src], 0);

    Dict_t* dist = malloc(graph->nbNode * sizeof(Dict_t));
    ASSERT(dist, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        Dict_init(&dist[i], dictSize, SEG_MAX);
    }


    Dict_add(&dist[src], 0, 0, 0, 0, NODE_SEGMENT, src);

    Extendable_list_t* extendable = NULL;

    extendable = Extendable_list_new(NULL, src, NULL);
    extendable->ext = Extendable_new(0, 0, 0, 0, NODE_SEGMENT, NULL);

    Extendable_t** nextextendable = malloc(graph->nbNode * sizeof(Extendable_t*));
    ASSERT(nextextendable, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        nextextendable[i] = NULL;
    }

    my_m2 tmp_igp = 0;


    (*pf) = malloc((maxIter+ 1) * sizeof(Dict_t*));
    ASSERT((*pf), -1, (maxIter+ 1));

    for (int i = 0 ; i <= maxIter ; i++) {
        (*pf)[i] = malloc(graph->nbNode * sizeof(Dict_t));
        ASSERT((*pf)[i], -1, graph->nbNode);
        for (int j = 0 ; j < graph->nbNode ; j++) {
            Dict_init((*pf)[i] + j, dictSize, 10);
        }
    }

    Dict_add(&(*pf)[0][src], 0, 0, 0, 0, NODE_SEGMENT, src);

    if (iters != NULL) {
        for (int i = 0 ; i < graph->nbNode ; i++) {
            (*iters)[i] = -1;
            minIgp[i] = INF;
        }
        (*iters)[src] = 0;
    }



    while (extendable != NULL && nbIter < sr->nbNode) {
        printf("Test\n");
        #pragma omp parallel for
        for (int dst = 0 ; dst < graph->nbNode ; dst++) {

            if (dst == src) {
                continue;
            }

            Dict_t pf_cand;
            Dict_init(&pf_cand, dictSize, SEG_MAX);
            Pfront_t pfcandlist;
            Pfront_init(&pfcandlist, dictSize);
            int t = 0;
            my_m1 imax = 0;

            //extend paths
            Best2cop_extend_path(dst, extendable, &pf_cand, &pfcandlist, dist + dst, graph, sr, &t, &imax, cstrM1, cstrM2);
            nextextendable[dst] = NULL;

            Best2cop_cpt_extendable_paths(nextextendable + dst, pfront, &pf_cand, dist + dst, &pfcandlist, t, imax, nbIter, dst, &(*pf)[nbIter][dst]);

            Dict_free(&pf_cand);
            Pfront_free(&pfcandlist);
        }

        Extendable_list_free(extendable);
        extendable = NULL;

        for (int i = 0 ; i < graph->nbNode ; i++) {
            //printf("iter : %d -> node %d\n", nbIter, i);
            if (nextextendable[i] != NULL) {
                if (iters != NULL) {
                    if (analyse == ANALYSE_DCLC) {
                        //Extendable_print(nextextendable[i]);
                        //
                        tmp_igp = nextextendable[i]->infos.m2;
                        if (tmp_igp < minIgp[i]) {
                            (*iters)[i] = nbIter;
                            minIgp[i] = tmp_igp;
                        }
                        
                    }

                    if (analyse == ANALYSE_2COP) {
                        (*iters)[i] = nbIter;
                    }
                }
                extendable = Extendable_list_new(extendable, i, NULL);
                extendable->ext = Extendable_copy(nextextendable[i]);
                Extendable_free(nextextendable[i]);
            }
        }

        nbIter++;
    }
    

    Extendable_list_free(extendable);

    for (int j = 0 ; j < graph->nbNode ; j++) {
        Dict_free(&dist[j]);
    }
    free(dist);
    free(nextextendable);
    free(minIgp);

    return nbIter - 1;
}


my_m2 update_min_igp(my_m2 old, Extendable_t* nextext) {
    if (nextext == NULL) {
        return old;
    }

    return MIN(nextext->infos.m2, update_min_igp(old, nextext->next));
}

char isNotOnDAG(SrGraph_t *sr_conv, int currDag, int actNode, int dst, LinkInfos *tmp)
{
    return ((sr_conv->m1dists[currDag][actNode] + tmp->m1 != sr_conv->m1dists[currDag][dst]) || (sr_conv->m2dists[currDag][actNode] + tmp->m2 != sr_conv->m2dists[currDag][dst]));
}

int addNecessarySegments(SrGraph_t *sr_conv, int *currDag, int actNode, int dst, LinkInfos *tmp, int *d0v, int lastSegment)
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

void Best2cop_extend_path(int dst, Extendable_list_t* extendable, Dict_t* pf_cand, Pfront_t* pfcandlist, 
                            Dict_t* dist_v, Topology_t* graph, SrGraph_t* sr_conv, int* t, my_m1* imax, my_m1 c1, my_m2 c2)
{
    *imax = 0;
    *t = 0;
    for (Extendable_list_t* d_list = extendable ; d_list != NULL ; d_list = d_list->next) {
        short edgeSrc = d_list->node;
        for (Extendable_t* path = d_list->ext ; path != NULL ; path = path->next) {
            for (Llist_t* edge = graph->pred[dst]; edge != NULL ; edge = edge->next) {
                my_m1 d1v = path->infos.m1 + edge->infos.m1;
                my_m2 d2v = path->infos.m2 + edge->infos.m2;
                my_m0 d0v = path->infos.m0;
                int currDag = path->infos.currDag;
                int lastSegment = path->infos.lastSegment;

                path->infos.lastSegment = addNecessarySegments(sr_conv, &currDag, edgeSrc, dst, &edge->infos, &d0v, lastSegment);

                // TODO: remettre les déchets (plus ou moins , mais peut être pas nécessaire)
                // TODO: Opti ?
                if (d1v < c1 && d2v < c2 && d0v <= SEG_MAX) {     
                    Dict_add(dist_v, d0v, d1v, d2v, currDag, lastSegment, edgeSrc);

                    if (pf_cand->paths[d0v][d1v].m2 == INF) {
                        Pfront_insert_key(pfcandlist, d1v, d0v);
                        *t = *t + 1;
                    }

                    Dict_add(pf_cand, d0v, d1v, d2v, currDag, lastSegment, 0);
                    *imax = MAX(*imax, d1v);
                }
            }
        }
    }
}


void Best2cop_cpt_extendable_paths(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, __attribute__ ((unused)) Pfront_t* pfcandlist, __attribute__ ((unused)) int t, 
                                    int imax, int iter, int dst, Dict_t* pf)
{
    // if (t * log(t) + t + (*pfront)[iter-1][dst].heapSize < imax / 10) {
    //     if (dst == 8) {
    //         printf("On est dans select\n");
    //     }
    //     Best2cop_cpt_extendable_paths_select(nextextendable, pfront, pf_cand, dist_v, pfcandlist, iter, dst, pf);
    // } else {
    //     Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax, pf);
    // }

    //printf("%d\n", t);
    Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax, pf);
}


// void Best2cop_cpt_extendable_paths_select(Extendable_t** nextextendable, Pfront_t*** pfront, 
//                                     Dict_t* pf_cand, Dict_t* dist_v, Pfront_t* pfcandlist, int iter, int dst, ParetoFront_t** pf)
// {
//     Pfront_sort(pfcandlist);
//     Pfront_t* d1_it = Pfront_merge_sort(&(*pfront)[iter-1][dst], pfcandlist);
//     if (dst == 8) {
//         Pfront_print(d1_it, stdout);
//     }

//     my_m2 last_d2 = INF;
//     for (int i = 0 ; i < d1_it->heapSize ; i++) {
//         my_m1 d1 = d1_it->keys[i];
//         if (dist_v->paths[d1] < last_d2) {
//             last_d2 = dist_v->paths[d1];
//             Pfront_insert_key(&(*pfront)[iter][dst], d1);
//             (*pf) = ParetoFront_new((*pf), d1, dist_v->paths[d1]);
//             if (pf_cand->paths[d1] != INF) {
//                 (*nextextendable) = Extendable_new(d1, pf_cand->paths[d1], (*nextextendable));
//             }
//         }
//     }

//     Pfront_free(d1_it);
//     free(d1_it);
// }


void Best2cop_cpt_extendable_paths_all(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, int iter, int dst, int imax, Dict_t* pf)
{
    UNUSED(imax);
    UNUSED(iter);

   // my_m1* dist = malloc(dist_v->max_m1 * sizeof(my_m1));
    for (my_m0 i = 0 ; i < dist_v->max_m0 ; i++) {
        my_m2 last_d2 = INF;
        for (my_m1 j = 0 ; j < dist_v->max_m1 ; j++) {
            if (dist_v->paths[i][j].m2 < last_d2) {
                last_d2 = dist_v->paths[i][j].m2;
                if (i < dist_v->max_m0 - 1 && dist_v->paths[i+1][j].m2 <= dist_v->paths[i][j].m2) {
                    dist_v->paths[i+1][j] = dist_v->paths[i][j];
                }
               
                
                Pfront_insert_key(&(*pfront)[iter][dst], i,j);
                if (pf_cand->paths[i][j].m2 != INF) {
                    Dict_add(pf, i,j, dist_v->paths[i][j].m2, dist_v->paths[i][j].currDag, dist_v->paths[i][j].lastSegment, 0);
                    pf->nb_add++;
                    (*nextextendable) = Extendable_new(i, j, pf_cand->paths[i][j].m2,dist_v->paths[i][j].currDag, dist_v->paths[i][j].lastSegment ,(*nextextendable));
                }
            }
        }
    }

    /*
    for (my_m0 i = 0 ; i < dist_v->max_m0 ; i++) {
        my_m2 last_d2 = INF;
        for (my_m1 j = 0 ; j < dist_v->max_m1 ; j++) {
            if (dist_v->paths[i][j].m2 < last_d2 && dist_v->paths[i][j].m0 ) {
                last_d2 = dist_v->paths[i];
                Pfront_insert_key(&(*pfront)[iter][dst], i);
                if (pf_cand->paths[i] != INF) {
                    Dict_add(pf, i, dist_v->paths[i][j], dist_v->preds[i][j]);
                    pf->nb_add++;
                    (*nextextendable) = Extendable_new(i, j, pf_cand->paths[i][j], pf_cand->paths[i][j] ,(*nextextendable));
                }
            }
        }
    }*/
}
