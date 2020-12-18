#include <stdio.h>
#include "include/base.h"
#include "include/Llist.h"
#include "include/LabelTable.h"
#include "include/Topology.h"
#include "include/ScopeStack.h"
#include "include/SrGraph.h"
#include "include/Dict.h"
#include "include/BinHeap.h"
#include "include/Best2cop.h"

#define NB_NODE         1000


int main(int argc, char** argv)
{
    if (argc != 2) {
        return 1;
    }

    SrGraph_t* sr = NULL;
    ParetoFront_t*** dist = NULL;
    BinHeap_t** pfront = NULL;
    struct timeval start, stop;
    FILE* output = fopen(argv[1], "w");
    fprintf(output, "C2 TIME\n");

    for (int i = 100 ; i <= 1000 ; i += 100)
    {
        sr = SrGraph_create_random_topo(NB_NODE, i);
        INFO("SR Graph with spread %d has been successfully loaded\n", i);

        for (int j = 0 ; j < NB_NODE / 10 ; j++) {
            dist = NULL;
            pfront = NULL;

            gettimeofday(&start, NULL);

            Best2cop(&pfront, &dist, sr, j, 1000, INF, 1000, 0, NULL);

            gettimeofday(&stop, NULL);

            for (int l = 0 ; l <= SEG_MAX ; l++) {
                for (int k = 0 ; k < NB_NODE ; k++) {
                    BinHeap_free(&pfront[l][k]);
                    ParetoFront_free(dist[l][k]);
                }
                free(pfront[l]);
                free(dist[l]);
            }

            free(pfront);
            free(dist);

            fprintf(output, "%d %ld\n", i, (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        }

        SrGraph_free(sr);
    }

    fclose(output);
    return 0;
}

