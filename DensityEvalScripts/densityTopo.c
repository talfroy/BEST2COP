#include <stdio.h>
#include "../include/base.h"
#include "../include/Llist.h"
#include "../include/LabelTable.h"
#include "../include/Topology.h"
#include "../include/ScopeStack.h"
#include "../include/SrGraph.h"
#include "../include/Dict.h"
#include "../include/BinHeap.h"
#include "../include/Best2cop.h"

#define NB_NODE 1000
#define NB_FILE 30
#define NB_OUTPUT 10
#define EVALNAME "DENSITY"
#define XP_PATH "/home/jro/Documents/EvalLCvsLS"
#define MAX_M1 100

long int Main_get_mean(long int *times, int size);

int Main_get_worst(long int *means, int size);

int main()
{

    SrGraph_t *sr[NB_FILE];
    struct timeval start, stop;
    Dict_t **dist = NULL;
    Pfront_t **pfront = NULL;
    long int *times;
    long int means[NB_FILE];
    memset(means, 0, NB_FILE * sizeof(long int));
    int nbIter;
    char fileName[128];
    char worstFile[128];
    char timeFile[128];
    FILE *fileRes = NULL;
    FILE *worstTopo = NULL;
    my_m1 dictSize = MIN(1000, MAX_M1 * SEG_MAX);

    int size = 1000;

    omp_set_num_threads(omp_get_max_threads());

    for (int density = 1; density <= 100; density += 20)
    {
        memset(worstFile, 0, 128);
        sprintf(worstFile, "%s/%s/resultsSpread%d/worstTopo%d.isp",XP_PATH, EVALNAME, MAX_M1, size);
        times = malloc(size / 10 * sizeof(long int));

        for (int j = 0; j < NB_FILE; j++)
        {
            //printf("Start of the loop\n");
            memset(fileName, 0, 128);
            sprintf(fileName, "%s/%s/resultsSpread%d/topo%d/topoSR%d.isp",XP_PATH, EVALNAME, MAX_M1, density, j);
            sr[j] = SrGraph_load_with_id(fileName, size, 0, 0);
            memset(timeFile, 0, 128);
            sprintf(timeFile, "%s/%s/resultsSpread%d/resultsTime%d/timeTopoSR%d.res",XP_PATH, EVALNAME, MAX_M1, density, j);
            fileRes = fopen(timeFile, "w");
            //printf("End of the initialization\n");

            for (int k = 0; k < 30; k++)
            {

                dist = NULL;
                pfront = NULL;
                //printf("Avant best2cop %d %d\n", j, k);

                gettimeofday(&start, NULL);

                nbIter = Best2cop(&pfront, &dist, sr[j], k, 1000, INF, dictSize + 1, 0, NULL);

                gettimeofday(&stop, NULL);
                //printf("Après best2cop %d %d\n", j, k);

                if (nbIter == 0)
                {
                    printf("y a un soucis\n");
                }

                times[k] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

                fprintf(fileRes, "%d %ld\n", nbIter, times[k]);

                for (int l = 0; l <= SEG_MAX; l++)
                {
                    for (int k = 0; k < size; k++)
                    {
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

            means[j] = Main_get_mean(times, size / 10);
            //printf("Mean = %ld\n", means[j]);
        }

        int indexWorst = Main_get_worst(means, NB_FILE);
        printf("For topo of density %d the worst is %d\n", density, indexWorst);
        worstTopo = fopen(fileName, "w");
        SrGraph_print_in_file(sr[indexWorst], worstTopo);

        fclose(worstTopo);

        for (int j = 0; j < NB_FILE; j++)
        {
            SrGraph_free(sr[j]);
        }
    }

    free(times);
}

long int Main_get_mean(long int *times, int size)
{
    long int mean = 0;
    for (int i = 0; i < size; i++)
    {
        mean += times[i];
    }

    mean /= size;
    return mean;
}

int Main_get_worst(long int *means, int size)
{
    int maxIndex = 0;
    long int max = means[0];

    for (int i = 1; i < size; i++)
    {
        if (max < means[i])
        {
            max = means[i];
            maxIndex = i;
        }
    }

    return maxIndex;
}
