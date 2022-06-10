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
#include "include/Compact.h"
#include "include/Segment_list.h"
#include "include/my_printf.h"
#include "include/Pfront.h"

//struct Options opt;

void max_of_tab(FILE *output, long int *tab, int **tabIter, int size, char full, int **isFeasible, struct Options opt);

void print_all_iter(FILE *output, int *tab, int size);


void Main_display_results(FILE *output, Dict_t **dist, int nbNodes, Pfront_t **heap, int iter);

void Main_display_all_paths(FILE *output, ParetoFront_t ***dist, int nbNodes, int iter);


void main_display_distances(FILE *out, Dict_t **dist, int iter, int nbNodes, int src,
                            Topology_t *topo, struct segment_list ***sl);

int main(int argc, char **argv)
{


    if (Option_command_parser(argc, argv) == -1)
    {
        usage(argv[0]);
        return 1;
    }

    if (opt.nbThreads != -1)
    {
        omp_set_num_threads(opt.nbThreads);
    }
    else
    {
        omp_set_num_threads(1);
    }

    SrGraph_t *sr = NULL;
    SrGraph_t *sr_conv = NULL;
    Topology_t *topo = NULL;
    FILE *output = stdout;
    FILE *resFile = NULL;

    if (opt.resFile)
    {
        resFile = fopen(opt.resFile, "w");
    }
    struct timeval start, stop;

#if OS_TYPE == LINUX
    register_printf_function('S', print_seglist, print_seglist_arginfo);
#endif

   

    LabelTable_t labels;
    LabelTable_init(&labels);

     if (opt.output != NULL)
    {
        output = fopen(opt.output, "w");
        if (output == NULL)
        {
            output = freopen(opt.output, "w", output);
            if (output == NULL)
            {
                ERROR("Output file does not exists\n");
            }
        }
    }

    if (opt.labelsOrId == LOAD_LABELS)
    {
        sr = SrGraph_load_with_label(opt.filename, opt.accuracy, opt.biDir, &labels);
        if (opt.srConv)
        {
            if (opt.srBinConv)
                sr_conv = SrGraph_load_bin(opt.srConv);
            else
                sr_conv = SrGraph_load_with_label(opt.filename, opt.accuracy, opt.biDir, &labels);
        }
        else
        {
            topo = Topology_load_from_file_labels(opt.filename, opt.accuracy, opt.biDir, &labels);
            sr_conv = SrGraph_create_from_topology_best_m2(topo);
        }
    }

    else
    {
        topo = Topology_load_from_file_ids(opt.filename, opt.accuracy, opt.biDir);
        sr = SrGraph_load_with_id(opt.filename, topo->nbNode, opt.accuracy, opt.biDir);
        if (opt.srConv)
        {
            if (opt.srBinConv)
                sr_conv = SrGraph_load_bin(opt.srConv);
            else
                sr_conv = SrGraph_load_with_id(opt.filename, topo->nbNode, opt.accuracy, opt.biDir);
        }
        else
        {
            sr_conv = SrGraph_create_from_topology_best_m2(topo);
        }
    }

    my_m1 maxSpread = SrGraph_get_max_spread(sr);

    if (maxSpread == -1)
    {
        INFO("Segment Routing graph has been transform into one connexe component\n");
        sr = SrGraph_get_biggest_connexe_coponent(sr);
        maxSpread = SrGraph_get_max_spread(sr);
    }

    //SrGraph_print_in_file_labels(sr, stdout, topo->labels);
    maxSpread *= SEG_MAX;
    opt.cstr1 *= my_pow(10, opt.accuracy);
    my_m1 max_dict_size = MIN(maxSpread, opt.cstr1);
    if (opt.saveSrGraphBin != NULL)
    {
        SrGraph_save_bin(sr_conv, opt.saveSrGraphBin);
    }

    if (max_dict_size >= DICT_LIMIT)
    {
        ERROR("MaxSpread is too high. Please reduce the accuracy or your computer will probably die. "
              "You can also modifie the DICT_LIMIT value in include/params.h at line 44 if you are confident\n");
    }
    Dict_t **pf = NULL;
    Pfront_t **pfront = NULL;

    if (opt.allNodes)
    {
        opt.allNodes = MIN(opt.allNodes, sr->nbNode);
    } 

    if (opt.allNodes && !opt.nb_areas)
    {
        long int *times = malloc(opt.allNodes * sizeof(long int));
        int **iters = malloc(opt.allNodes * sizeof(int *));
        int *iterMax = calloc(opt.allNodes, sizeof(int));
        int **isFeasible = malloc(opt.allNodes * sizeof(int *));
        

        for (int i = 0; i < opt.allNodes; i++)
        {
            times[i] = 0;
            iters[i] = calloc(sr->nbNode, sizeof(int));
            isFeasible[i] = malloc(sr->nbNode * sizeof(int));
            pf = NULL;
            pfront = NULL;
            int iteri = 0;
            int init_time = 0;
            gettimeofday(&start, NULL);
            iteri = Best2cop(&pfront, &pf, sr, sr_conv, i, opt.cstr1, opt.cstr2, max_dict_size, opt.analyse, &iters[i], opt.bascule, &init_time);

            gettimeofday(&stop, NULL);
            iterMax[i] = MAX(iteri, iterMax[i]);
            times[i] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
            times[i] -= init_time;
            //RESULTS("Iter max : %d\n", iterMax[i]);
            // struct segment_list ***sl = Segment_list_retreive_paths(pf, sr, iterMax[i], i);
            if (resFile)
                main_display_distances(resFile, pf, iterMax[i], sr->nbNode, i, topo, NULL);
            //segment_list_free(sl, iter, sr->nbNode);
            //printf("\r");

            for (int j = 0; j < iteri; j++)
            {
                for (int k = 0; k < sr->nbNode; k++)
                {
                    Pfront_free(&pfront[j][k], SEG_MAX);
                    Dict_free(&pf[j][k]);
                }
                free(pfront[j]);
                free(pf[j]);
            }

            free(pfront);
            free(pf);
        }
        // struct segment_list ***sl = Segment_list_retreive_paths(pf, sr, iter, opt.src);
        max_of_tab(output, times, &iterMax, opt.allNodes, opt.analyse, isFeasible, opt);
        for (int i = 0; i < opt.allNodes; i++)
        {
            free(iters[i]);
            free(isFeasible[i]);
        }
        free(iterMax);
        free(isFeasible);
        free(times);
        free(iters);
    }
    else if (opt.src != -1 || opt.src_lab != NULL)
    {
        if (opt.src == -1)
        {
            opt.src = LabelTable_get_id(&labels, opt.src_lab);
        }
        pf = NULL;
        pfront = NULL;
        int *itersSolo = malloc(sr->nbNode * sizeof(int));
        // int test = 0;
        // Best2cop(&pfront, &pf, sr, sr_conv, opt.src, opt.cstr1, opt.cstr2, max_dict_size + 1, opt.analyse, NULL, opt.bascule, &test);
        pf = NULL;
        pfront = NULL;

        //printf("params\nsrc = %d\ncstr1 = %d\ncstr2 = %d\ndict size = %d\nmaxSpread = %d\n", opt.src, opt.cstr1, opt.cstr2, max_dict_size, maxSpread);
        int init_time = 0;
        gettimeofday(&start, NULL);

        int iter = Best2cop(&pfront, &pf, sr, sr_conv, opt.src, opt.cstr1, opt.cstr2, max_dict_size + 1, opt.analyse, &itersSolo, opt.bascule, &init_time);

        gettimeofday(&stop, NULL);
        long int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
        time -= init_time;
        if (resFile)
            main_display_distances(resFile, pf, iter, sr->nbNode, opt.src, topo, NULL);

        if (opt.analyse)
        {
            fprintf(output, "Execution stop after %d iterations\n", iter);
        }
        else
        {
            RESULTS("! Execution takes %ld us\n", time);
        }

       

        // TO PRINT RESULTS
       // struct segment_list ***sl = Segment_list_retreive_paths(pf, sr, iter, opt.src);
       main_display_distances(output, pf, iter, sr->nbNode, opt.src,topo, NULL);
        for (int j = 0; j < iter; j++)
        {
            for (int k = 0; k < sr->nbNode; k++)
            {
                Pfront_free(&pfront[j][k], SEG_MAX);
                Dict_free(&pf[j][k]);
            }
            free(pfront[j]);
            free(pf[j]);
        }
        if (opt.analyse)
        {
            for (int i = 0; i < sr->nbNode; i++)
            {
                fprintf(output, "%d %d %d\n", opt.src, i, itersSolo[i]);
            }
        }

        // segment_list_free(sl, iter, sr->nbNode);
        free(pfront);
        free(pf);
        free(itersSolo);
    }
   
    else
    {
        ERROR("Choose an available loading mode\n");
    }

    if (opt.interface)
    {
        free(opt.filename);
    }

    if (!opt.nb_areas)
    {
        SrGraph_free(sr);
        SrGraph_free(sr_conv);
    }
    if (resFile)
    {
        fclose(resFile);
    }
    return 0;
}


void Main_display_all_paths(FILE *output, ParetoFront_t ***dist, int nbNodes, int iter)
{
    for (int i = 0; i < nbNodes; i++)
    {
        for (int j = iter - 1; j > 0; j--)
        {
            if (dist[j][i] != NULL)
            {
                for (ParetoFront_t *tmp = dist[j][i]; tmp != NULL; tmp = tmp->next)
                {
                    fprintf(output, "%d %d\n", tmp->m1, tmp->m2);
                }
                break;
            }
        }
    }
}

void max_of_tab(FILE *output, long int *tab, int **tabIter, int size, char full, int **isFeasible, struct Options opt)
{
    if (full)
    {
        fprintf(output, "NODE_ID DEST C2 NB_ITER IS_FEASIBLE\n");
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                fprintf(output, "%d %d %d %d %d\n", i, j, opt.cstr1, tabIter[i][j], isFeasible[i][j]);
            }
        }
    }
    else
    {
        fprintf(output, "NODE_ID C2 NB_THREADS TIME ITER\n");
        for (int i = 0; i < size; i++)
        {
            fprintf(output, "%d %d %d %ld %d\n", i, opt.cstr1, opt.nbThreads, tab[i], (*tabIter)[i]);
        }
    }
}



void main_display_area_sr_time_mean(long int *times, int nb_areas)
{
    long int tot, mean, square;
    tot = 0;
    for (int i = 1; i < nb_areas - 1; i++)
    {
        tot += times[i];
    }

    mean = tot / (nb_areas - 2);

    tot = 0;
    for (int i = 1; i < nb_areas - 1; i++)
    {
        tot += (mean - times[i]) * (mean - times[i]);
    }
    square = tot / ((nb_areas - 2) * 2);
    square = sqrt(square);

    RESULTS("Mean time for areas transformation is %ld us\n", mean);
    RESULTS("Standard deviation (95%%) for areas transformation is %ld us\n", 2 * square);
}

void main_display_distances(FILE *out, Dict_t **dist, int iter, int nbNodes, int src,
                            Topology_t *topo, struct segment_list ***sl)
{
    UNUSED(sl);
    UNUSED(topo);
    UNUSED(src);
    UNUSED(iter);
    for (int i=0; i<2; i++){
        for (int j = 0; j < nbNodes; j++)
        {
            for (int k = 0; k < dist[i]->max_m0 ; k++)
            {
                for (int l = 0; l < dist[i]->max_m1 ; l++)
                {
                    if (dist[i][j].paths[k][l].m2 != INF)
                    {
                        //fprintf(out, "%s %s %d %d %d", LabelTable_get_name(topo->labels, src),
                        //LabelTable_get_name(topo->labels, j), i, k, dist[i][j].paths[k]);
                        //segment_list_invert(&sl[i][j][k]);
                        //Segment_list_print(out, &sl[i][j][k], topo, NULL);
                        //Segment_list_print_id(out, &sl[i][j][k]);
                        //fprintf(out, "\n");
                        //fprintf(out, "%d %d %d %d\n", j, k, dist[i][j].paths[k], i);
                        fprintf(out, "%d %d %d %d\n", j, l, dist[i][j].paths[k][l].m2, k);
                    }
                }
            }
        } 
    }              
}

