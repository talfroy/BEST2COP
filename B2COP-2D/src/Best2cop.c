#include "../include/Best2cop.h"



int Best2cop(Pfront_t*** pfront, Dict_t*** pf, SrGraph_t* graph, int src, my_m1 cstrM1, 
            my_m2 cstrM2, my_m1 dictSize, char analyse, int** iters, int bascule, int* init_time)
{

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
            Pfront_init(&(*pfront)[i][j], dictSize);
        }
    }

    // Init first distance
    Pfront_insert_key(&(*pfront)[0][src], 0);

    Dict_t* dist = malloc(graph->nbNode * sizeof(Dict_t));
    ASSERT(dist, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        Dict_init(&dist[i], dictSize);
    }

    Dict_add(&dist[src], 0, 0, src);

    // Create extendable of first distance
    Extendable_list_t** extendable = calloc(graph->nbNode,sizeof(Extendable_list_t*));

    ASSERT(extendable, -1, graph->nbNode);
    IntList_t* succs = graph->nonEmptySlots[src];
    int active_nodes[graph->nbNode];
    int active_nodes_nb = 0;

    // We only try to extend to v if the link u->v exists
    while(succs != NULL)
    {
        int succ = succs->value;
        if (succ == src)
        {
            continue;
        }
        if (extendable[succ] == NULL) {
            extendable[succ] = Extendable_list_new(NULL, src, NULL);
            extendable[succ]->ext = Extendable_new(0, 0, NULL);
        }
        active_nodes[active_nodes_nb] = succ;
        active_nodes_nb++;
        succs = succs->next;
    }

    Extendable_t** nextextendable = malloc(graph->nbNode * sizeof(Extendable_t*));
    ASSERT(nextextendable, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        nextextendable[i] = NULL;
    }


    (*pf) = malloc((2) * sizeof(Dict_t*));
    ASSERT((*pf), -1, (2));

    for (int i = 0 ; i < 2 ; i++) {
        (*pf)[i] = malloc(graph->nbNode * sizeof(Dict_t));
        ASSERT((*pf)[i], -1, graph->nbNode);
        for (int j = 0 ; j < graph->nbNode ; j++) {
            Dict_init((*pf)[i] + j, dictSize);
        }
    }

    Dict_add(&(*pf)[0][src], 0, 0, src);

    if (iters != NULL) {
        for (int i = 0 ; i < graph->nbNode ; i++) {
            (*iters)[i] = -1;
            minIgp[i] = INF;
        }
        (*iters)[src] = 0;
    }
    bool to_extend = true;
    gettimeofday(&stop, NULL);
    *init_time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    while (to_extend && nbIter <= maxIter) {
       	#pragma omp parallel for schedule(dynamic)
        for (int idst = 0 ; idst < active_nodes_nb ; idst++) {
            int dst = active_nodes[idst];

            if (dst == src) {
                continue;
            }
            Dict_t pf_cand;
            Dict_init(&pf_cand, dictSize);
            Pfront_t pfcandlist;
            Pfront_init(&pfcandlist, dictSize);
            int t = 0;
            my_m1 imax = 0;

            Best2cop_extend_path(dst, extendable[dst], &pf_cand, &pfcandlist, dist + dst, graph, &t, &imax, cstrM1, cstrM2);

            nextextendable[dst] = NULL;

            Best2cop_cpt_extendable_paths(nextextendable+dst, pfront, &pf_cand, dist + dst, &pfcandlist, t, imax, nbIter, dst, &(*pf)[nbIter%2][dst], bascule);

            Dict_free(&pf_cand);
            Pfront_free(&pfcandlist);
        }

        for (int j = 0; j < graph->nbNode; j++) {
            Extendable_list_free(extendable[j]);
            extendable[j] = NULL;
        }


        // path were extended by/ƒmaxiƒto i, must be extended by/to succs of i next time
        to_extend = false;
        int next_active_nodes_nb = 0;
        int next_active_nodes[graph->nbNode];

        IntList_t* succs;
        for (int i = 0 ; i < active_nodes_nb ; i++) {
            int node = active_nodes[i];
            if (nextextendable[node] != NULL) {
                succs = graph->nonEmptySlots[node];
                while(succs != NULL)
                {
                    int succ = succs->value;
                    if (succ == node)
                    {
                        succs = succs->next;
                        continue;
                    }

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
               Extendable_free(nextextendable[node]);
               nextextendable[node]=NULL;
            }
        } 

        active_nodes_nb = next_active_nodes_nb;
        for (int i = 0 ; i < active_nodes_nb ; i++) {
            active_nodes[i] = next_active_nodes[i];
        }

        nbIter++;
    }
    
    gettimeofday(&start, NULL);

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

void Best2cop_extend_path(int dst, Extendable_list_t* extendable, Dict_t* pf_cand, Pfront_t* pfcandlist, 
                            Dict_t* dist_v, SrGraph_t* graph, int* t, my_m1* imax, my_m1 c1, my_m2 c2)
{
    *imax = 0;
    *t = 0;
    int count = 0;
    for (Extendable_list_t* d_list = extendable ; d_list != NULL ; d_list = d_list->next) {
        short edgeSrc = d_list->node;
        for (Extendable_t* path = d_list->ext ; path != NULL ; path = path->next) {
            for (Edge_t* edge = graph->pred[dst][edgeSrc] ; edge != NULL ; edge = edge->next) {
                count ++;
                my_m1 d1v = path->infos.m1 + edge->m1;
                my_m2 d2v = path->infos.m2 + edge->m2;
                if (d1v < c1 && d2v < c2 && dist_v->paths[d1v] > d2v) {
                    Dict_add(dist_v, d1v, d2v, edgeSrc);

                    if (pf_cand->paths[d1v] == INF) {
                        Pfront_insert_key(pfcandlist, d1v);
                        *t = *t + 1;
                    }

                    Dict_add(pf_cand, d1v, d2v, 0);
                    *imax = MAX(*imax, d1v);
                }

                
            }
        }
    }
}


void Best2cop_cpt_extendable_paths(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, __attribute__ ((unused)) Pfront_t* pfcandlist, __attribute__ ((unused)) int t, 
                                    int imax, int iter, int dst, Dict_t* pf, int bascule)
{
    UNUSED(imax);
    UNUSED(pf);

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


    if (bascule) {
        if (t  + (*pfront)[iter-1][dst].heapSize < bascule) {
            Best2cop_cpt_extendable_paths_select(nextextendable, pfront, pf_cand, dist_v, pfcandlist, iter, dst, pf);
        } else {
            Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax, pf);
        }
    } else {
        Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax, pf);
    }
}


void Best2cop_cpt_extendable_paths_select(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, Pfront_t* pfcandlist, int iter, int dst, Dict_t* pf)
{
    // UNUSED(pf_cand);
    // int pfrontIndex = 0;
    // int pfcandIndex = 0;
    // my_m2 last_d2 = INF;
    // Pfront_t* pfront2 = &(*pfront)[iter-1][dst];
    // Pfront_sort(pfcandlist);
    // while (pfrontIndex < pfront2->heapSize && pfcandIndex < pfcandlist->heapSize) {
    //     if (pfront2->keys[pfrontIndex] < pfcandlist->keys[pfcandIndex]) {
    //         my_m1 d1 = pfront2->keys[pfrontIndex];
    //         my_m2 d2 = dist_v->paths[d1];
    //         if (d2 < last_d2) {
    //             Pfront_insert_key(&(*pfront)[iter][dst], d1);
    //             last_d2 = d2;
    //         }
    //         pfrontIndex++;
    //     } else {
    //         my_m1 d1 = pfcandlist->keys[pfcandIndex];
    //         my_m2 d2 = dist_v->paths[d1];
    //         if (d2 < last_d2) {
    //             Pfront_insert_key(&(*pfront)[iter][dst], d1);
    //             last_d2 = d2;
    //            // if (pf_cand->paths[d1] != INF) {
    //             Dict_add(pf, d1, d2, dist_v->preds[d1]);
    //             (*nextextendable) = Extendable_new(d1, d2, (*nextextendable));
    //             pf->nb_add++;
    //            // }
    //         }
    //         pfcandIndex++;
    //         if (pfront2->keys[pfrontIndex] == pfcandlist->keys[pfcandIndex]) {
    //             pfrontIndex++;
    //         }
    //     }
    // }

    // while (pfrontIndex < pfront2->heapSize) {
    //     my_m1 d1 = pfront2->keys[pfrontIndex];
    //     my_m2 d2 = dist_v->paths[d1];
    //     if (d2 < last_d2) {
    //         Pfront_insert_key(&(*pfront)[iter][dst], d1);
    //         last_d2 = d2;
    //     }
    //     pfrontIndex++;
    // }

    // while (pfcandIndex < pfcandlist->heapSize) {
    //     my_m1 d1 = pfcandlist->keys[pfcandIndex];
    //     my_m2 d2 = dist_v->paths[d1];
    //     if (d2 < last_d2) {
    //         Pfront_insert_key(&(*pfront)[iter][dst], d1);
    //         last_d2 = d2;
    //         Dict_add(pf, d1, d2, dist_v->preds[d1]);
    //         (*nextextendable) = Extendable_new(d1, d2, (*nextextendable));
    //         pf->nb_add++;
    //     }
    //     pfcandIndex++;
    // }

    Pfront_sort(pfcandlist);
    Pfront_t* d1_it = Pfront_merge_sort(&(*pfront)[iter-1][dst], pfcandlist);
    my_m2 last_d2 = INF;
    for (int i = 0 ; i < d1_it->heapSize ; i++) {
        my_m1 d1 = d1_it->keys[i];
        my_m2 d2 = dist_v->paths[d1];
        
        if (d2 < last_d2) {
            Pfront_insert_key(&(*pfront)[iter][dst], d1);
            last_d2 = d2;
            
            if (pf_cand->paths[d1] != INF) {
                Dict_add(pf, d1, d2, dist_v->preds[d1]);
                (*nextextendable) = Extendable_new(d1, d2, (*nextextendable));
                 pf->nb_add++;
            }
        }
    }
   Pfront_free(d1_it);
   free(d1_it);
}

void Best2cop_cpt_extendable_paths_all(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, int iter, int dst, int imax, Dict_t* pf)
{
    UNUSED(imax);
    my_m2 last_d2 = INF;
    
    for (my_m1 i = 0 ; i < dist_v->size ; i++) {
        if (dist_v->paths[i] < last_d2) {
            last_d2 = dist_v->paths[i];
            Pfront_insert_key(&(*pfront)[iter%2][dst], i);
            if (pf_cand->paths[i] != INF) {
                Dict_add(pf, i, dist_v->paths[i], dist_v->preds[i]);
                pf->nb_add++;
                (*nextextendable) = Extendable_new(i, pf_cand->paths[i], (*nextextendable));
            }
        }
    }
}

