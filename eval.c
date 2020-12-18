#include <stdio.h>
#include "include/base.h"
#include "include/Llist.h"
#include "include/LabelTable.h"
#include "include/Topology.h"
#include "include/ScopeStack.h"
#include "include/SrGraph.h"
#include "include/Dict.h"
#include "include/BinHeap.h"
#include "include/CrashTest.h"


int main(int argc, char** argv)
{
    if (argc != 5) {
        printf("Usage : ./eval [nbNodes] [nbParaLinks] [delayCstr] [nbThreads]\n");
        return 1;
    }

    int nbNodes = atoi(argv[1]);
    int nbPl = atoi(argv[2]);
    int cstr = atoi(argv[3]);
    int threads = atoi(argv[4]);
    omp_set_num_threads(threads);
    SrGraph_t* sr = SrGraph_create_crash_test(nbNodes, nbPl);
    if (sr != NULL) {
        INFO("Crash Test graph with %d node and %d Constraint has been succefully loaded\n", nbNodes, cstr);
    }


    struct timeval start, stop;
    gettimeofday(&start, NULL);

    //CrashTest(sr, cstr, nbPl, spread);
    CrashTest(sr, cstr, nbPl, 1000);

    gettimeofday(&stop, NULL);

       // printf("Execution took %lu us\nNb Iter : %ld\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec, nb_iter);

    fprintf(stdout, "%d %d %d %f\n", nbNodes, nbPl, threads, (float)((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec)/1000000);
    SrGraph_free(sr);
    return 0;
}

