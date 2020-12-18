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


int main(int argc, char** argv)
{
    if (argc != 3) {
        return 1;
    }

    FILE* output = fopen(argv[2], "w");
    ParetoFront_t*** dist = NULL;
    BinHeap_t** pfront = NULL;
    SrGraph_t* sr = SrGraph_load_with_id(argv[1], 1000, 1, 0);
    int maxSpread = SrGraph_get_max_spread(sr);
    struct timeval start, stop;
    int dictSize;
    fprintf(output, "SOURCE C2 TIME\n");

    for (int i = 50 ; i <= 1000 ; i += 50) {
        printf("Now with gamma = %d\n", i);
        dictSize = MIN(i, maxSpread * SEG_MAX);
        for (int j = 0 ; j < sr->nbNode ; j++) {
            dist = NULL;
            pfront = NULL;

            gettimeofday(&start, NULL);

            Best2cop(&pfront, &dist, sr, j, i, INF, dictSize, 0, NULL);

            gettimeofday(&stop, NULL);

            for (int l = 0 ; l <= SEG_MAX ; l++) {
                for (int k = 0 ; k < sr->nbNode ; k++) {
                    BinHeap_free(&pfront[l][k]);
                    ParetoFront_free(dist[l][k]);
                }
                free(pfront[l]);
                free(dist[l]);
            }

            free(pfront);
            free(dist);

            fprintf(output, "%d %d %ld\n", j, i, (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        }
    }

    SrGraph_free(sr);
    return 0;
}

