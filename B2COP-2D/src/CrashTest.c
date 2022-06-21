#include "../include/CrashTest.h"


long int CrashTest(SrGraph_t* graph, my_m1 cstrM1, int nbPlLinks, int realSpread)
{
    printf("CrashTest is not up to date with the remaining of the project\n");
    exit(1);
    
    int src = 0;
    Dict_t* dist = malloc((size_t)graph->nbNode * sizeof(Dict_t));
    ASSERT(dist, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        Dict_init(dist + i, cstrM1 + 1);
    }

    Dict_add(dist + src, 0, 0, 0);

    Pfront_t** pfront = malloc((size_t)(SEG_MAX + 1) * sizeof(Pfront_t*));
    ASSERT(pfront, -1, (SEG_MAX + 1));

    for (int i = 0 ; i <= SEG_MAX ; i++) {
        pfront[i] = malloc((size_t)graph->nbNode * sizeof(Pfront_t));
        ASSERT(pfront[i], -1, graph->nbNode);
        for (int j = 0 ; j < graph->nbNode ; j++) {
            Pfront_init(&pfront[i][j], cstrM1 + 1);
        }
    }

    Pfront_insert_key(&pfront[0][src], 0);

    Extendable_list_t* extendable = Extendable_list_create();

    Extendable_list_add(extendable, src, NULL);
    extendable->ext[0] = Extendable_create();
    Extendable_add(extendable->ext[0], 0, 0);

    Extendable_t** nextextendable = malloc((size_t)graph->nbNode * sizeof(Extendable_t*));
    ASSERT(nextextendable, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        nextextendable[i] = NULL;
    }

    int nbIter = 1;
    int maxIter = SEG_MAX;


    while(extendable != NULL && nbIter <= maxIter) {
        //printf("Step : %d\n", nbIter);

        #pragma omp parallel for schedule (dynamic)
        for (int dst = 0 ; dst < graph->nbNode ; dst++) {

            if (dst == src) {
                continue;
            }

            Dict_t pf_cand;
            Dict_init(&pf_cand, cstrM1 + 1);
            Pfront_t pfcandlist;
            Pfront_init(&pfcandlist, cstrM1 + 1);

            CrashTest_extend_path(graph, extendable, dist + dst, &pf_cand, &pfcandlist, dst);

            nextextendable[dst] = NULL;

            CrashTest_cpt_extendable_paths(nextextendable + dst, &pfront, realSpread, dist + dst, nbPlLinks, dst, graph->nbNode, nbIter, cstrM1);

            Dict_free(&pf_cand);
            Pfront_free(&pfcandlist);
        }

        Extendable_list_free(extendable);
        extendable = NULL;

        for (int i = 0 ; i < graph->nbNode ; i++) {
            if (nextextendable[i] != NULL) {
                // TODO: fix this
                //Extendable_list_add(extendable, i, Extendable_copy(nextextendable[i]));
                Extendable_free(nextextendable[i]);
            }
        }

        nbIter++;
    }

    Extendable_list_free(extendable);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        Dict_free(dist + i);
    }

    free(dist);

    free(nextextendable);

    for (int i = 0 ; i <= SEG_MAX ; i++) {
        for (int j = 0 ; j < graph->nbNode ; j++) {
            Pfront_free(&pfront[i][j]);
        }
        free(pfront[i]);
    }

    free(pfront);

    return nbIter;
}


void CrashTest_extend_path(SrGraph_t* graph, Extendable_list_t* extendable, Dict_t* dist_v, Dict_t* pf_cand, Pfront_t* pfcandlist, int dst)
{
    for(size_t ext_list_id = 0 ; ext_list_id < extendable->size ; ext_list_id++) {

        Extendable_t* path = extendable->ext[ext_list_id];
        int edgeSrc = extendable->sources[ext_list_id];
        
        for (size_t path_idx = 0; path_idx < path->size ;path_idx++) {
            for (Edge_t* edge = graph->pred[dst][edgeSrc] ; edge != NULL ; edge = edge->next) {
                my_m1 d1v = path->infos[path_idx].m1 + edge->m1;
                my_m2 d2v = path->infos[path_idx].m2 + edge->m2;

                Dict_add(dist_v, d1v, d2v, edgeSrc);
                Dict_add(pf_cand, d1v, d2v, edgeSrc);
                Pfront_insert_key(pfcandlist, d1v);
            }
        }
    }
}


void CrashTest_cpt_extendable_paths(Extendable_t** nextextendable, Pfront_t*** pfront, int realSpread, Dict_t* dist_v, 
                                    int dst, int nbPara, int nbNode, int currStep, my_m1 cstrM1)
{
    my_m1 nbIter;
    if (currStep < 3) {
        nbIter = MIN(TO_M1(my_pow(nbNode, currStep - 1) * my_pow(nbPara, currStep)), TO_M1(currStep * realSpread));
        nbIter = MIN(nbIter, cstrM1);
    } else {
        nbIter = MIN(cstrM1, TO_M1(currStep * realSpread));
    }

    for (my_m1 i = 0 ; i < nbIter ; i++) {
        if (dist_v->paths[i] != M2_INF) {
            Pfront_insert_key(&(*pfront)[currStep][dst], i);
            Extendable_add((*nextextendable), 1, 1);
        }
    }
}