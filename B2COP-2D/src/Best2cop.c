#include "../include/Best2cop.h"



int Best2cop(Pfront_t*** pfront, Dict_t** pf, SrGraph_t* graph, int src, my_m1 cstrM1, 
            my_m2 cstrM2, size_t dictSize, char analyse, int** iters, size_t bascule, long* init_time)
{

    struct timeval start, stop;
    gettimeofday(&start, NULL);

    //Start of init
    // No SR : V-1 iteration at most
    int maxIter = graph->nbNode - 1;
    my_m2* minIgp = malloc((size_t)graph->nbNode * sizeof(my_m2));

    if (analyse) {
        maxIter = MAX(maxIter, SEG_MULT * SEG_MAX);
    }

    int nbIter = 1;

    // Two pfront because not-in-place
    (*pfront) = malloc((2) * sizeof(Pfront_t*));
    ASSERT(pfront, -1, (2));
    // One pfront per node
    for (int i = 0 ; i < 2 ; i++) {
        (*pfront)[i] = malloc((size_t)graph->nbNode * sizeof(Pfront_t));
        ASSERT((*pfront)[i], -1, graph->nbNode);

        for (int j = 0 ; j < graph->nbNode ; j++) {
            Pfront_init(&(*pfront)[i][j], dictSize);
        }
    }

    // Init first distance
    
    Pfront_insert_key(&(*pfront)[0][src], TO_M1(0));
 


    Dict_t* dist = malloc((size_t)graph->nbNode * sizeof(Dict_t));
    ASSERT(dist, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        Dict_init(&dist[i], dictSize);
    }

    Dict_add(&dist[src], TO_M1(0), TO_M2(0), src);

    // Create extendable of first distance
    Extendable_list_t** extendable = malloc((size_t)graph->nbNode * sizeof(*extendable));
    ASSERT(extendable, -1, graph->nbNode);
    for(int i = 0 ; i < graph->nbNode ; i++) {
        extendable[i] = Extendable_list_create();
    }
       // Create extendable of first distance
    Extendable_t** extendable_paths = malloc((size_t)graph->nbNode * sizeof(*extendable_paths));
    ASSERT(extendable_paths, -1, graph->nbNode);
    for(int i = 0 ; i < graph->nbNode ; i++) {
        extendable_paths[i] = Extendable_create();
    }

    Extendable_t** next_extendable_paths = malloc((size_t)graph->nbNode * sizeof(Extendable_t*));
    ASSERT(next_extendable_paths, -1, graph->nbNode);
    for (int i = 0 ; i < graph->nbNode ; i++) {
        next_extendable_paths[i] = Extendable_create();
    }


    // We only try to extend to v if the link u->v exists
    {
        IntList_t* succs  = graph->nonEmptySlots[src];
        while(succs != NULL)
        {
            int succ = succs->value;
            if (extendable[succ]->size == 0) {
                Extendable_list_add(extendable[succ], src, extendable_paths[src]);
                Extendable_add(extendable_paths[src], TO_M1(0), TO_M2(0));
                //printf("init extendable for %d (%zu)\n", succ, extendable[succ]->size);
            }
            succs = succs->next;
        }
    }


    if (iters != NULL) {
        for (int i = 0 ; i < graph->nbNode ; i++) {
            (*iters)[i] = -1;
            minIgp[i] = M2_INF;
        }
        (*iters)[src] = 0;
    }
    bool to_extend = true;
    // Dict_t pf_cand;
    // Dict_init(&pf_cand, dictSize);
    Dict_t pf_cand_all[8];
    for (int i = 0 ; i < 8 ; i++) {
        Dict_init(&pf_cand_all[i], dictSize);
    }

    gettimeofday(&stop, NULL);
    *init_time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    while (to_extend && nbIter <= maxIter) {

        //printf("--------- ITERATION %i\n", nbIter);
       	#pragma omp parallel for schedule(dynamic)
        //for (int idst = 0 ; idst < active_nodes_nb ; idst++) {
        //    int dst = active_nodes[idst];
        for (int dst = 0 ; dst < graph->nbNode ; dst++) {    
            if (dst == src) {
                continue;
            }
            // ignore if there is no extendable path towards dst
            if (Extendable_list_size(extendable[dst]) == 0) {
                continue;
            }
            //printf("extend to -> %d ? (%zu)\n", dst, Extendable_list_size(extendable[dst]));
            Dict_t pf_cand = pf_cand_all[omp_get_thread_num()];
            // Dict_t pf_cand;
            // Dict_init(&pf_cand, dictSize);
            Dict_reset(&pf_cand);
            Pfront_t pfcandlist;
            Pfront_init(&pfcandlist, dictSize);
            size_t t = 0;
            my_m1 imax = TO_M1(0);

            Best2cop_extend_path(dst, extendable[dst], &pf_cand, &pfcandlist, dist + dst, graph, &t, &imax, cstrM1, cstrM2);

            Best2cop_cpt_extendable_paths(&next_extendable_paths[dst], pfront, &pf_cand, dist + dst, &pfcandlist, t, imax, nbIter, dst, bascule);

            // Dict_free(&pf_cand);
            Pfront_free(&pfcandlist);
        }

        for (int j = 0; j < graph->nbNode; j++) {
            Extendable_clear(extendable_paths[j]);
        }
        {
            //swap extendable_paths and next_extendable_paths
            Extendable_t** tmp = extendable_paths;
            extendable_paths = next_extendable_paths;
            next_extendable_paths = tmp;

        }

        for (int i = 0 ; i < graph->nbNode ; i++) {
            if (Extendable_list_size(extendable[i]) > 0) {
                Extendable_list_clear(extendable[i]);
            }
        }

        // path were extended by/ƒmaxiƒto i, must be extended by/to succs of i next time
        to_extend = false;

        IntList_t* succs;
        for (int node = 0 ; node < graph->nbNode ; node++) {

            if (extendable_paths[node]->size > 0) {
                succs = graph->nonEmptySlots[node];
                while(succs != NULL)
                {
                    int succ = succs->value;
                    if (succ == node)
                    {
                        succs = succs->next;
                        continue;
                    }

                    to_extend = true;
                    Extendable_list_add(extendable[succ], node, extendable_paths[node]); 
                    succs = succs->next;
                }
            }
        } 


        nbIter++;
    }
    // printf("nbIter = %d\n", nbIter);
    gettimeofday(&start, NULL);


    (*pf) = malloc((size_t)graph->nbNode * sizeof(**pf));

    ASSERT((*pf), -1, graph->nbNode);

    for (int j = 0 ; j < graph->nbNode ; j++) {
        Dict_init((*pf) + j, dictSize);
        // copy dist into pf
        for(my_m1 k = 0 ; k < dist[j].size ; k++) {
            if (dist[j].paths[k] != M2_INF) {
                Dict_add((*pf) + j, k, dist[j].paths[k], dist[j].preds[k]);
                //printf("%i -> %i : %"M1_FMT"\n", j, i, dist[j].paths[i]);
            }
        }
    }

    for (int j = 0 ; j < graph->nbNode ; j++) {
        Dict_free(&dist[j]);
        Extendable_list_free(extendable[j]);
        Extendable_free(extendable_paths[j]);
        Extendable_free(next_extendable_paths[j]);
    }
    free(extendable);
    free(dist);
    free(next_extendable_paths);
    free(extendable_paths);
    free(minIgp);
    gettimeofday(&stop, NULL);
    *init_time += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    return 2;
}


my_m2 update_min_igp(Extendable_t* nextext) {
    my_m2 min_igp = M2_INF;
    for(size_t i = 0; nextext->size; i++){
        if (nextext->infos[i].m2 < min_igp) {
            min_igp = nextext->infos[i].m2;
        }
    }
    return min_igp;
}

void Best2cop_extend_path(int dst, Extendable_list_t* extendable, Dict_t* pf_cand, Pfront_t* pfcandlist, 
                            Dict_t* dist_v, SrGraph_t* graph, size_t* t, my_m1* imax, my_m1 c1, my_m2 c2)
{
    *imax = 0;
    *t = 0;
    int count = 0;
    for(size_t ext_list_id = 0 ; ext_list_id < extendable->size ; ext_list_id++) {

        Extendable_t* path = extendable->ext[ext_list_id];
        int edgeSrc = extendable->sources[ext_list_id];

        for (size_t path_idx = 0; path_idx < path->size ;path_idx++) {
            for (Edge_t* edge = graph->pred[dst][edgeSrc] ; edge != NULL ; edge = edge->next) {
                count ++;
                
                my_m1 d1v = path->infos[path_idx].m1 + edge->m1;
                my_m2 d2v = path->infos[path_idx].m2 + edge->m2;
              
                if (d1v <= c1 && d2v <= c2 && dist_v->paths[d1v] > d2v) {
                    //printf("extend path from %i -> %llu %llu (%llu, %llu)\n", edgeSrc, path->infos.m1, path->infos.m2, d1v, d2v);
                    Dict_add(dist_v, d1v, d2v, edgeSrc);

                    if (pf_cand->paths[d1v] == M2_INF) {
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
                                    Dict_t* pf_cand, Dict_t* dist_v, __attribute__ ((unused)) Pfront_t* pfcandlist, size_t t, 
                                    my_m1 imax, int iter, int dst, size_t bascule)
{
    UNUSED(imax);

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
            Best2cop_cpt_extendable_paths_select(nextextendable, pfront, pf_cand, dist_v, pfcandlist, iter, dst);
        } else {
            Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax);
        }
    } else {
        Best2cop_cpt_extendable_paths_all(nextextendable, pfront, pf_cand, dist_v, iter, dst, imax);
    }
}


void Best2cop_cpt_extendable_paths_select(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, Pfront_t* pfcandlist, int iter, int dst)
{
    // UNUSED(pf_cand);
    // int pfrontIndex = 0;
    // int pfcandIndex = 0;
    // my_m2 last_d2 = M2_INF;
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
    //            // if (pf_cand->paths[d1] != M2_INF) {
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
    my_m2 last_d2 = M2_INF;
    for (size_t i = 0 ; i < d1_it->heapSize ; i++) {
        my_m1 d1 = d1_it->keys[i];
        my_m2 d2 = dist_v->paths[d1];
        
        if (d2 < last_d2) {
            Pfront_insert_key(&(*pfront)[iter][dst], d1);
            last_d2 = d2;
            
            if (pf_cand->paths[d1] != M2_INF) {
                Extendable_add((*nextextendable), d1, d2);
            }
        }
    }
   Pfront_free(d1_it);
   free(d1_it);
}

void Best2cop_cpt_extendable_paths_all(Extendable_t** nextextendable, Pfront_t*** pfront, 
                                    Dict_t* pf_cand, Dict_t* dist_v, int iter, int dst, my_m1 imax)
{
    UNUSED(imax);
    my_m2 last_d2 = M2_INF;
    
    for (my_m1 i = 0 ; i < dist_v->size ; i++) {
        if (dist_v->paths[i] < last_d2) {
            //printf("path towards %i added (%llu %llu)\n", dst, i, dist_v->paths[i]);
            last_d2 = dist_v->paths[i];
            Pfront_insert_key(&(*pfront)[iter%2][dst], i);
            if (pf_cand->paths[i] != M2_INF) {
                Extendable_add((*nextextendable), i, pf_cand->paths[i]);
            }
        }
    }
}

