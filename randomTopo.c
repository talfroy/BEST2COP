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

#define NB_NODE     1000
#define NB_FILE     10
#define NB_OUTPUT   10



long int Main_get_mean(long int* times, int size);

int Main_get_worst(long int* means, int size);


int main (int argc, char**argv)
{
    if (argc != 2) {
        return 1;
    }

    int maxSpread = atoi(argv[1]);
    SrGraph_t* sr[NB_FILE];
    struct timeval start, stop;
    Dict_t** dist = NULL;
    Pfront_t** pfront = NULL;
    long int* times;
    long int means[NB_FILE];
    memset(means, 0, NB_FILE * sizeof(long int));
    int nbIter;
    char fileName[128];
    char worstFile[128];
    char timeFile[128];
    FILE* fileRes = NULL;
    FILE* worstTopo = NULL;
    my_m1 dictSize = MIN(1000, maxSpread * SEG_MAX);

    omp_set_num_threads(omp_get_max_threads());

    for (int i = 100 ; i <= 1000 ; i += 100) {
        memset(worstFile, 0, 128);
        sprintf(worstFile, "RND/resultsSpread%s/worstTopo%d.isp", argv[1], i);
        times = malloc(i / 10 * sizeof(long int));

        for (int j = 0 ; j < NB_FILE ; j++) {
            //printf("Start of the loop\n");
            memset(fileName, 0, 128);
            sprintf(fileName, "RND/resultsSpread%s/topo%d/topoSR%d.isp", argv[1], i, j);
            sr[j] = SrGraph_load_with_id(fileName, i, 0, 0);
            memset(timeFile, 0, 128);
            sprintf(timeFile, "RND/resultsSpread%s/resultsTime%d/timeTopoSR%d.res", argv[1], i, j);
            fileRes = fopen(timeFile, "w");
            //printf("End of the initialization\n");

            for (int k = 0 ; k < i / 10 ; k++) {
                
                dist = NULL;
                pfront = NULL;
                //printf("Avant best2cop %d %d\n", j, k);

                gettimeofday(&start, NULL);

                nbIter = Best2cop(&pfront, &dist, sr[j], k, 1000, INF, dictSize+1, 0, NULL);

                gettimeofday(&stop, NULL);
                //printf("Après best2cop %d %d\n", j, k);

                if (nbIter == 0) {
                    printf("y a un soucis\n");
                }

                times[k] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

                fprintf(fileRes, "%ld\n", times[k]);
                
                for (int l = 0 ; l <= SEG_MAX ; l++) {
                    for (int k = 0 ; k < i ; k++) {
                        Pfront_free(&pfront[l][k]);
                        Dict_free(&dist[l][k]);
                    }
                    free(pfront[l]);
                    free(dist[l]);
                }

                free(pfront);
                free(dist);
                //printf("Topo %d successfully computed, iter %d\n", j, k);
            }

            fclose(fileRes);

            means[j] = Main_get_mean(times, i / 10);
            //printf("Mean = %ld\n", means[j]);
        }


        int indexWorst = Main_get_worst(means, NB_FILE);
        printf("For topo of size %d the worst is %d\n", i, indexWorst);
        worstTopo = fopen(fileName, "w");
        SrGraph_print_in_file(sr[indexWorst], worstTopo);

        fclose(worstTopo);

        for (int j = 0 ; j < NB_FILE ; j++) {
            SrGraph_free(sr[j]);
        }
    }

    free(times);
}



long int Main_get_mean(long int* times, int size)
{
    long int mean = 0;
    for (int i = 0 ; i < size ; i++) {
        mean += times[i];
    }

    mean /= size;
    return mean;
}

int Main_get_worst(long int* means, int size)
{
    int maxIndex = 0;
    long int max = means[0];

    for (int i = 1 ; i < size ; i++) {
        if (max < means[i]) {
            max = means[i];
            maxIndex = i;
        }
    }

    return maxIndex;
}
