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


void max_of_tab(FILE* output, long int* tab, int** tabIter, int size, char full, int** isFeasible, struct Options opt);

void print_all_iter(FILE* output, int* tab, int size);

void Main_get_all_infos(struct Options* opt);

void Main_display_results(FILE* output, Dict_t** dist, int nbNodes, Pfront_t** heap, int iter);

void Main_display_all_paths(FILE* output, ParetoFront_t*** dist, int nbNodes, int iter);

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
        Main_get_all_infos(&opt);
    }

    if (opt.loadingMode == LOAD_TOPO) {
        topo = Topology_load_from_file(opt.filename, opt.accuracy, opt.biDir);

        if (topo == NULL) {
            ERROR("Topology can't be load\n");
            return EXIT_FAILURE;
        }

        INFO("Topology succesfully loaded\n");

        gettimeofday(&start, NULL);
        if (opt.flex) {
            sr = SrGraph_create_flex_algo(topo);
        } else {
            sr = SrGraph_create_from_topology_best_m2(topo);
        }
        gettimeofday(&stop, NULL);



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
    } else if (opt.loadingMode == LOAD_SR_BIN) { 
        sr = SrGraph_load_bin(opt.filename);
        if (sr == NULL) {
            ERROR("The Segment Routing Graph can't be loaded\n");
            return EXIT_FAILURE;
        }
        INFO("Segment Routing Graph succesfully loaded\n");
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

    if(opt.saveSrGraphBin != NULL)
    {
        SrGraph_save_bin(sr, opt.saveSrGraphBin);
    }

    my_m1 maxSpread = SrGraph_get_max_spread(sr);


    if (maxSpread == -1) {
        INFO("Segment Routing graph has been transform into one connexe component\n");
        sr = SrGraph_get_biggest_connexe_coponent(sr);
        maxSpread = SrGraph_get_max_spread(sr);
    }


    //SrGraph_print_in_file(sr, stdout);
    maxSpread *= SEG_MAX;
    opt.cstr1 *= my_pow(10, opt.accuracy);
    my_m1 max_dict_size = MIN(maxSpread, opt.cstr1);

    if (max_dict_size >= DICT_LIMIT) {
        ERROR("MaxSpread is too high. Please reduce the accuracy or your computer will probably die. " \
        "You can also modifie the DICT_LIMIT value in include/params.h at line 44 if you are confident\n");
    }

    Dict_t** pf = NULL;
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
            pf = NULL;
            pfront = NULL;

            //printf("Iter %d\n", i);
            gettimeofday(&start, NULL);

            iterMax[i] = Best2cop(&pfront, &pf, sr, i, opt.cstr1, opt.cstr2, max_dict_size, opt.analyse, &iters[i]);

            gettimeofday(&stop, NULL);

            //printf("Fin Iter %d\n", i);
            times[i] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
            //RESULTS("Iter max : %d\n", iterMax[i]);

            //printf("\r");

            if (opt.analyse) {
                maxIter = 10 * SEG_MAX;
            } else {
                maxIter = SEG_MAX ;
            }

            for (int j = 0 ; j < sr->nbNode ; j++) {
                if (iters[i][j] > SEG_MAX || iters[i][j] < 0) {
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
                    Dict_free(&pf[j][k]);
                }
                free(pfront[j]);
                free(pf[j]);
            }

            free(pfront);
            free(pf);
        }

        max_of_tab(output, times, iters, sr->nbNode, opt.analyse, isFeasible, opt);
        for (int i = 0 ; i < sr->nbNode ; i++) {
            free(iters[i]);
            free(isFeasible[i]);
        }
        free(iterMax);
        free(isFeasible);
        free(times);
        free(iters);
    } else {
        pf = NULL;
        pfront = NULL;
        int* itersSolo = malloc(sr->nbNode * sizeof(int));
        //printf("params\nsrc = %d\ncstr1 = %d\ncstr2 = %d\ndict size = %d\nmaxSpread = %d\n", opt.src, opt.cstr1, opt.cstr2, max_dict_size, maxSpread);
        gettimeofday(&start, NULL);

        int iter = Best2cop(&pfront, &pf, sr, opt.src, opt.cstr1, opt.cstr2, max_dict_size + 1, opt.analyse, &itersSolo);
        gettimeofday(&stop, NULL);
        long int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

        if (opt.analyse) {
            fprintf(output, "Execution stop after %d iterations\n", iter);
        } else {
            RESULTS("Execution takes %ld us\n", time);
        }
        

        //Main_display_results(output, pf, sr->nbNode, pfront, iter);
        //Main_display_all_paths(output, dist, sr->nbNode, iter);
        if (opt.analyse) {
            maxIter = 10 * SEG_MAX;
        } else {
            maxIter = SEG_MAX + 1;
        }
        return 0;
        struct segment_list*** sl = Dict_retreive_paths(pf, sr, iter, opt.src);
        print_segment_list(sl, iter, sr->nbNode);


        for (int j = 0 ; j < maxIter ; j++) {
            for (int k = 0 ; k < sr->nbNode ; k++) {
                Pfront_free(&pfront[j][k]);
                Dict_free(&pf[j][k]);
            }
            free(pfront[j]);
            free(pf[j]);
        }

        if (opt.analyse) {
            for (int i = 0 ; i < sr->nbNode ; i++) {
                fprintf(output, "%d %d %d\n", opt.src, i, itersSolo[i]);
            }
        }



        free(pfront);
        free(pf);
        free(itersSolo);
    }
    if (opt.interface) {
        free(opt.filename);
    }

    Topology_free(topo);
    SrGraph_free(sr);
    return 0;
}


void Main_display_results(FILE* output, Dict_t** dist, int nbNodes, __attribute__((unused)) Pfront_t** heap, int iter)
{
    for (int i = 0 ; i < nbNodes ; i++) {
        for (int k = 0 ; k < iter ; k++) {
            // for (ParetoFront_t* tmp = dist[k][i] ; tmp != NULL ; tmp = tmp->next) {
            //     fprintf(output, "%d %d %d %d\n", i, tmp->m1, tmp->m2, k);
            // }

            for (int j = 0 ; j < dist[k][i].size ; j++) {
                if (dist[k][i].paths[j] != INF) {
                    fprintf(output, "%d %d %d %d\n", i, j, dist[k][i].paths[j], k);
                }
            }
        }

    }
}

void Main_display_all_paths(FILE* output, ParetoFront_t*** dist, int nbNodes, int iter)
{
    for (int i = 0 ; i < nbNodes ; i++) {
        for (int j = iter-1 ; j > 0 ; j--) {
            if (dist[j][i] != NULL) {
                for (ParetoFront_t* tmp = dist[j][i] ; tmp != NULL ; tmp = tmp->next) {
                    fprintf(output, "%d %d %d\n", i, tmp->m1, tmp->m2);
                }
                break;
            }
        }
    }
}



void max_of_tab(FILE* output, long int* tab, int** tabIter, int size, char full, int** isFeasible, struct Options opt)
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

void Main_get_all_infos(struct Options* opt)
{
    printf("Enter your file name : \n");
    opt->filename = malloc(128);
    int scan = scanf("%s", opt->filename);
    if (scan == EOF) {
        return;
    }

    int get = 0;
    printf("Load as a topology file(1) or a Segment Routing Graph(2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF) {
        return;
    }
    opt->loadingMode = get-1;

    printf("Are the node in the file identified by Labels (1) or IDs (2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF) {
        return;
    }
    opt->labelsOrId = get - 1;

    printf("Do you want to compute on only one node (1) or all (2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF) {
        return;
    }
    opt->allNodes = get - 1;

    if (!opt->allNodes) {
        printf("Please enter the id of the source node\n");
        scan = scanf("%d", &get);
        if (scan == EOF) {
            return;
        }
        opt->src = get;
    }

}
