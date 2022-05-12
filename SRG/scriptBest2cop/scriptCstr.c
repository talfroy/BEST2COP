#include <stdio.h>
#include "include/Option.h"
#include "include/base.h"
#include "include/Llist.h"
#include "include/LabelTable.h"
#include "include/Topology.h"
#include "include/ScopeStack.h"
#include "include/SrGraph.h"
#include "include/Dict.h"
#include "include/BinHeap.h"
#include "include/Best2cop.h"

struct Options opt;

int main(int argc, char** argv)
{
    int c2s[28] = {50, 100, 500, 1000};

    if (argc != 3) {
        return 0;
    }

    FILE* output = fopen(argv[2], "w");
    Topology_t* topo = Topology_load_from_file(argv[1], 1, 0);
    SrGraph_t* sr = SrGraph_create_from_topology_best_m2(topo);
    my_m1 maxSpred = SrGraph_get_max_spread(sr);
    my_m1 dictSize = 0;
    struct timeval start, stop;
    Dict_t** dist = NULL;
    Pfront_t** pfront = NULL;

    int** iters = malloc(sr->nbNode * sizeof(int*));

    for (int i = 0 ; i < sr->nbNode ; i++) {
        iters[i] = malloc(sr->nbNode * sizeof(int));
    }

    for (int i = 0 ; i < 6 ; i++) {
        printf("Now with Gamma = %d\n", c2s[i]);
        dictSize = MIN(c2s[i], maxSpred * SEG_MAX);

        for (int j = 0 ; j < sr->nbNode ; j++) {
            dist = NULL;
            pfront = NULL;

            gettimeofday(&start, NULL);

            Best2cop(&pfront, &dist, sr, j, i, INF, dictSize, 1, &iters[j]);

            gettimeofday(&stop, NULL);

            for (int k = 0 ; k < sr->nbNode ; k++) {
                fprintf(output, "%d %d %d %d %ld\n", j, k, iters[j][k], c2s[i], (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
            }

            for (int l = 0 ; l <= 5*SEG_MAX ; l++) {
                for (int k = 0 ; k < sr->nbNode ; k++) {
                    Pfront_free(&pfront[l][k]);
                    Dict_free(&dist[l][k]);
                }
                free(pfront[l]);
                free(dist[l]);
            }

            free(pfront);
            free(dist);
        }
    }

    for (int i = 0 ; i < sr->nbNode ; i++) {
        free(iters[i]);
    }
    free(iters);

    fclose(output);
    Topology_free(topo);
    SrGraph_free(sr);
    return 0;
}
