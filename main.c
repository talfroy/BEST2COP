#include <stdio.h>
#include "include/base.h"
#include "include/Llist.h"
#include "include/LabelTable.h"
#include "include/Topology.h"
#include "include/ScopeStack.h"
#include "include/SrGraph.h"
#include "include/Dict.h"
#include "include/Option.h"
#include "include/Best2cop.h"

//struct Options opt;


void max_of_tab(FILE* output, long int* tab, int** tabIter, int size, char full, int** isFeasible);

void print_all_iter(FILE* output, int* tab, int size);

void Main_get_all_infos();

void Main_display_results(FILE* output, ParetoFront_t*** dist, int nbNodes, Pfront_t** heap, int iter);

int main(int argc, char** argv)
{
    if (Option_command_parser(argc, argv) == -1) {
        usage(argv[0]);
        return 1;
    }

    if (opt.nbThreads != -1) {
        omp_set_num_threads(opt.nbThreads);
    } else {
        omp_set_num_threads(omp_get_max_threads());
    }

    Topology_t* topo = NULL;
    SrGraph_t* sr = NULL;
    FILE* output = stdout;
    struct timeval start, stop;

    if (opt.interface) {
        Main_get_all_infos();
    }

    if (opt.loadingMode == LOAD_TOPO) {
        topo = Topology_load_from_file(opt.filename, opt.accuracy, opt.biDir);

        if (topo == NULL) {
            ERROR("Topology can't be load\n");
            return EXIT_FAILURE;
        }

        INFO("Topology succesfully loaded\n");

        gettimeofday(&start, NULL);
        sr = SrGraph_create_from_topology_best_m2(topo);
        gettimeofday(&stop, NULL);
        //SrGraph_print_in_file(sr, stdout);



        if (sr == NULL) {
            ERROR("The Segment Routing Graph can't be computed\n");
            Topology_free(topo);
            return EXIT_FAILURE;
        }

        INFO("Segment Routing Graph succesfully computed\n");
        INFO("Tranformation takes %ld us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    } else if (opt.loadingMode == LOAD_SR) {
        if (opt.labelsOrId == LOAD_IDS) {
            sr = SrGraph_load_with_id(opt.filename, MAX_SR_GRAPH_SIZE, opt.accuracy, opt.biDir);

            if (sr == NULL) {
                ERROR("The Segment Routing Graph can't be loaded\n");
                return EXIT_FAILURE;
            }

            INFO("Segment Routing Graph succesfully loaded\n");
        } else if (opt.labelsOrId == LOAD_LABELS) {
            sr = SrGraph_load_with_label(opt.filename, opt.accuracy, opt.biDir);

            if (sr == NULL) {
                ERROR("The Segment Routing Graph can't be loaded\n");
                return EXIT_FAILURE;
            }

            INFO("Segment Routing Graph succesfully loaded\n");
        }
    } else if (opt.randomTopo) {
        topo = Topology_create_random(100, NULL, NULL);
        INFO("Topology succesfully loaded\n");

        gettimeofday(&start, NULL);
        sr = SrGraph_create_from_topology_best_m2(topo);
        gettimeofday(&stop, NULL);



        if (sr == NULL) {
            ERROR("The Segment Routing Graph can't be computed\n");
            Topology_free(topo);
            return EXIT_FAILURE;
        }

        INFO("Segment Routing Graph succesfully computed\n");
        INFO("Tranformation takes %ld us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

        //SrGraph_print_in_file(sr, stdout);
    } else {
        ERROR("Please choose an available loading mode\n");
    }

    ASSERT(sr, 1, "Segment Routing Graph hasn't been loaded yet so we kill the program\n");

    if (opt.output != NULL) {
        output = fopen(opt.output, "w");
        if (output == NULL) {
            output = freopen(opt.output, "w", output);
            if (output == NULL) {
                ERROR("Output file does not exists\n");
            }
        }
    }

    my_m1 maxSpread = SrGraph_get_max_spread(sr);

    if (maxSpread == -1) {
        INFO("Segment Routing graph has been transform into one connexe component\n");
        sr = SrGraph_get_biggest_connexe_coponent(sr);
        maxSpread = SrGraph_get_max_spread(sr);
    }


    maxSpread *= SEG_MAX;
    opt.cstr1 *= my_pow(10, opt.accuracy);
    my_m1 max_dict_size = MIN(maxSpread, opt.cstr1);

    if (max_dict_size >= DICT_LIMIT) {
        ERROR("MaxSpread is too high. Please reduce the accuracy or your computer will probably die. " \
        "You can also modifie the DICT_LIMIT value in include/params.h at line 44 if you are confident\n");
    }

    ParetoFront_t*** dist = NULL;
    Pfront_t** pfront = NULL;
    int maxIter = 0;

    if (opt.allNodes) {
        long int* times = malloc(sr->nbNode * sizeof(long int));
        int** iters = malloc(sr->nbNode * sizeof(int*));
        int* iterMax = malloc(sr->nbNode * sizeof(int));
        int** isFeasible = malloc(sr->nbNode * sizeof(int*));

        for (int i = 0 ; i < sr->nbNode ; i++) {
            //printf("Iter %d\n", i);
            times[i] = 0;
            iters[i] = malloc(sr->nbNode * sizeof(int));
            isFeasible[i] = malloc(sr->nbNode * sizeof(int));
            dist = NULL;
            pfront = NULL;
            gettimeofday(&start, NULL);

            iterMax[i] = Best2cop(&pfront, &dist, sr, i, opt.cstr1, opt.cstr2, max_dict_size, opt.full, &iters[i]);

            gettimeofday(&stop, NULL);
            times[i] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

            //printf("\r");

            if (opt.full) {
                maxIter = 5 * SEG_MAX;
            } else {
                maxIter = SEG_MAX ;
            }

            for (int j = 0 ; j < sr->nbNode ; j++) {
                if (iters[i][j] > SEG_MAX) {
                    if (!hasapath(pfront, j)) {
                        isFeasible[i][j] = 0;
                    } else {
                        isFeasible[i][j] = 1;
                    }
                } else {
                    isFeasible[i][j] = 1;
                }


            }

            for (int j = 0 ; j <= maxIter ; j++) {
                for (int k = 0 ; k < sr->nbNode ; k++) {
                    Pfront_free(&pfront[j][k]);
                    ParetoFront_free(dist[j][k]);
                }
                free(pfront[j]);
                free(dist[j]);
            }

            free(pfront);
            free(dist);
        }

        max_of_tab(output, times, iters, sr->nbNode, opt.full, isFeasible);
        for (int i = 0 ; i < sr->nbNode ; i++) {
            free(iters[i]);
            free(isFeasible[i]);
        }
        free(iterMax);
        free(isFeasible);
        free(times);
        free(iters);
    } else {
        dist = NULL;
        pfront = NULL;
        //printf("params\nsrc = %d\ncstr1 = %d\ncstr2 = %d\ndict size = %d\nmaxSpread = %d\n", opt.src, opt.cstr1, opt.cstr2, max_dict_size, maxSpread);
        gettimeofday(&start, NULL);

        int iter = Best2cop(&pfront, &dist, sr, opt.src, opt.cstr1, opt.cstr2, max_dict_size + 1, opt.full, NULL);

        gettimeofday(&stop, NULL);
        long int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

        if (opt.full) {
            fprintf(output, "Execution stop after %d iterations\n", iter);
        }
        fprintf(output, "Execution takes %ld us\n", time);

        //Main_display_results(output, dist, sr->nbNode, pfront, iter);
        if (opt.full) {
            maxIter = sr->nbNode;
        } else {
            maxIter = SEG_MAX + 1;
        }

        for (int j = 0 ; j < maxIter ; j++) {
            for (int k = 0 ; k < sr->nbNode ; k++) {
                Pfront_free(&pfront[j][k]);
                ParetoFront_free(dist[j][k]);
            }
            free(pfront[j]);
            free(dist[j]);
        }

        free(pfront);
        free(dist);
    }
    if (opt.interface) {
        free(opt.filename);
    }

    Topology_free(topo);
    SrGraph_free(sr);
    return 0;
}


void Main_display_results(FILE* output, ParetoFront_t*** dist, int nbNodes, __attribute__((unused)) Pfront_t** heap, int iter)
{
    for (int i = 0 ; i < nbNodes ; i++) {
        for (int k = 0 ; k < iter ; k++) {
            for (ParetoFront_t* tmp = dist[k][i] ; tmp != NULL ; tmp = tmp->next) {
                fprintf(output, "%d %d %d %d\n", i, tmp->m1, tmp->m2, k);
            }
        }

    }
}



void max_of_tab(FILE* output, long int* tab, int** tabIter, int size, char full, int** isFeasible)
{
    if (full) {
        fprintf(output, "NODE_ID DEST C2 NB_ITER IS_FEASIBLE\n");
        for (int i = 0 ; i < size ; i++) {
            for (int j = 0 ; j < size ; j++) {
                fprintf(output, "%d %d %d %d %d\n", i, j, opt.cstr1, tabIter[i][j], isFeasible[i][j]);
            }
        }
    } else {
        fprintf(output, "NODE_ID C2 NB_THREADS TIME\n");
    for (int i = 0 ; i < size ; i++) {
        fprintf(output, "%d %d %d %ld\n", i, opt.cstr1, opt.nbThreads, tab[i]);
    }
    }
}

void Main_get_all_infos()
{
    printf("Enter your file name : \n");
    opt.filename = malloc(128);
    int scan = scanf("%s", opt.filename);
    if (scan == EOF) {
        return;
    }

    int get = 0;
    printf("Load as a topology file(1) or a Segment Routing Graph(2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF) {
        return;
    }
    opt.loadingMode = get-1;

    printf("Are the node in the file identified by Labels (1) or IDs (2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF) {
        return;
    }
    opt.labelsOrId = get - 1;

    printf("Do you want to compute on only one node (1) or all (2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF) {
        return;
    }
    opt.allNodes = get - 1;

    if (!opt.allNodes) {
        printf("Please enter the id of the source node\n");
        scan = scanf("%d", &get);
        if (scan == EOF) {
            return;
        }
        opt.src = get;
    }

}
