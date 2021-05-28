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


int main()
{
    int spread[12] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 500, 1000};
    long int means[12];
    SrGraph_t* sr = NULL;
    Topology_t* topo = NULL;
    ParetoFront_t*** dist = NULL;
    BinHeap_t** pfront = NULL;
    struct timeval start, stop;
    char filename[128];
    long int mean = 0;
    int dictSize = 0;
    FILE* output = NULL;

    output = fopen("figure6/resultsSpreadSprint.txt", "w");
    fprintf(output, "SPREAD TIME\n");
    
    for (int i = 0 ; i < 2 ; i++) {
        memset(filename, 0, 128);
        sprintf(filename, "topos/sprint-modif%d.isp", spread[i]);
        topo = Topology_load_from_file(filename, 1, 0);
        printf("file %s loaded\n", filename);
        sr = SrGraph_create_from_topology(topo);
        if (sr != NULL) {
            printf("Sr loaded with %d nodes\n", sr->nbNode);
        }
        mean = 0;
        my_m1 maxspread = SrGraph_get_max_spread(sr);

        dictSize = MIN(maxspread * SEG_MAX , 1000);

        for (int j = 0 ; j < sr->nbNode / 10 ; j++) {
            dist = NULL;
            pfront= NULL;

            gettimeofday(&start, NULL);

            Best2cop(&pfront, &dist, sr, j, 1000, INF, dictSize, 0, NULL);

            gettimeofday(&stop, NULL);

            mean += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
            fprintf(output, "%d %ld\n", spread[i], (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

            for (int w = 0 ; w <= SEG_MAX ; w++) {
                for (int z = 0 ; z < sr->nbNode; z++) {
                    BinHeap_free(&pfront[w][z]);
                    ParetoFront_free(dist[w][z]);
                }
                free(pfront[w]);
                free(dist[w]);
            }

            free(pfront);
            free(dist);
        }
        printf("end of file\n");
        mean /= sr->nbNode / 10;
        means[i] = mean;

        Topology_free(topo);
        SrGraph_free(sr);
    }
    int worstIndex = 0;
    long int worst = means[0];
    for (int i = 1 ; i < 12 ; i++) {
        if (worst < means[i]) {
            worst = means[i];
            worstIndex = i;
        }
    }

    fclose(output);

    printf("The worst sprint is with spread %d\n", spread[worstIndex]);

    return 0;
}

