#include "../include/CrashTest.h"


long int CrashTest(SrGraph_t* graph, my_m1 cstrM1, int nbPlLinks, int realSpread)
{
    int src = 0;
    Dict_t* dist = malloc(graph->nbNode * sizeof(Dict_t));
    ASSERT(dist, -1, graph->nbNode);

    for (int i = 0 ; i < graph->nbNode ; i++) {
        Dict_init(dist + i, cstrM1 + 1);
    }

    Dict_add(dist + src, 0, 0, 0);

    Pfront_t** pfront = malloc((SEG_MAX + 1) * sizeof(Pfront_t*));
    ASSERT(pfront, -1, (SEG_MAX + 1));

    for (int i = 0 ; i <= SEG_MAX ; i++) {
        pfront[i] = malloc(graph->nbNode * sizeof(Pfront_t));
        ASSERT(pfront[i], -1, graph->nbNode);
        for (int j = 0 ; j < graph->nbNode ; j++) {
            Pfront_init(&pfront[i][j], cstrM1 + 1);
        }
    }

    Pfront_insert_key(&pfront[0][src], 0);

    Extendable_list_t* extendable = NULL;

    extendable = Extendable_list_new(NULL, src, NULL);
    extendable->ext = Extendable_new(0, 0, NULL);

    Extendable_t** nextextendable = malloc(graph->nbNode * sizeof(Extendable_t*));
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
                extendable = Extendable_list_new(extendable, i, NULL);
                extendable->ext = Extendable_copy(nextextendable[i]);
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
    for (Extendable_list_t* d_list = extendable ; d_list != NULL ; d_list = d_list->next) {
        short edgeSrc = d_list->node;
        for (Extendable_t* path = d_list->ext ; path != NULL ; path = path->next) {
            for (Edge_t* edge = graph->pred[dst][edgeSrc] ; edge != NULL ; edge = edge->next) {
                my_m1 d1v = path->infos.m1 + edge->m1;
                my_m2 d2v = path->infos.m2 + edge->m2;

                Dict_add(dist_v, d1v, d2v, edgeSrc);
                Dict_add(pf_cand, d1v, d2v, edgeSrc);
                Pfront_insert_key(pfcandlist, d1v);
            }
        }
    }
}


void CrashTest_cpt_extendable_paths(Extendable_t** nextextendable, Pfront_t*** pfront, int realSpread, Dict_t* dist_v, 
                                    int dst, int nbPara, int nbNode, int currStep, int cstrM1)
{
    long int nbIter;
    if (currStep < 3) {
        nbIter = MIN(my_pow(nbNode, currStep - 1) * my_pow(nbPara, currStep), currStep * realSpread);
        nbIter = MIN(nbIter, cstrM1);
    } else {
        nbIter = MIN(cstrM1, currStep * realSpread);
    }

    for (int i = 0 ; i < nbIter ; i++) {
        if (dist_v->paths[i] != -1) {
            Pfront_insert_key(&(*pfront)[currStep][dst], i);
            (*nextextendable) = Extendable_new(1, 1, (*nextextendable));
        }
    }
}