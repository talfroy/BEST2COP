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

//struct Options opt;

void max_of_tab(FILE *output, long int *tab, int **tabIter, int size, char full, int **isFeasible, struct Options opt);

void print_all_iter(FILE *output, int *tab, int size);

void Main_get_all_infos(struct Options *opt);

void Main_display_results(FILE *output, Dict_t **dist, int nbNodes, Pfront_t **heap, int iter);

void Main_display_all_paths(FILE *output, ParetoFront_t ***dist, int nbNodes, int iter);

void main_display_area_time_mean(long int *times, int nb_areas);

void main_display_area_sr_time_mean(long int *times, int nb_areas);

void main_display_distances(FILE *out, Dict_t **dist, int iter, int nbNodes, int src,
                            Topology_t *topo, struct segment_list ***sl);

int run_acc_cartesian_product(Topology_t **areas, SrGraph_t **sr_areas, FILE *output);

void run_cart_on_specific_ABR(Topology_t **areas, SrGraph_t **sr_areas,
                              FILE *output, int abr_area, int abr_id);

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
        omp_set_num_threads(omp_get_max_threads());
    }

    Topology_t *topo = NULL;
    Topology_t **areas = NULL;
    SrGraph_t *sr = NULL;
    SrGraph_t **sr_areas = NULL;
    FILE *output = stdout;
    struct timeval start, stop;

#if OS_TYPE == LINUX
    register_printf_function('S', print_seglist, print_seglist_arginfo);
#endif

    if (opt.interface)
    {
        Main_get_all_infos(&opt);
    }

    if (opt.loadingMode == LOAD_TOPO)
    {
        if (opt.labelsOrId == LOAD_LABELS) 
            topo = Topology_load_from_file_labels(opt.filename, opt.accuracy, opt.biDir);
        else 
            topo = Topology_load_from_file_ids(opt.filename, opt.accuracy, opt.biDir);


        if (topo == NULL)
        {
            ERROR("Topology can't be load\n");
            return EXIT_FAILURE;
        }

        INFO("Topology succesfully loaded\n");

        gettimeofday(&start, NULL);
        if (opt.flex)
        {
            sr = SrGraph_create_flex_algo(topo);
        }
        else
        {
            sr = SrGraph_create_from_topology_best_m2(topo);
        }
        gettimeofday(&stop, NULL);

        if (sr == NULL)
        {
            ERROR("The Segment Routing Graph can't be computed\n");
            Topology_free(topo);
            return EXIT_FAILURE;
        }

        INFO("Segment Routing Graph succesfully computed\n");
        INFO("Tranformation takes %ld us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    else if (opt.loadingMode == LOAD_SR)
    {
        if (opt.labelsOrId == LOAD_IDS)
        {
            sr = SrGraph_load_with_id(opt.filename, MAX_SR_GRAPH_SIZE, opt.accuracy, opt.biDir);

            if (sr == NULL)
            {
                ERROR("The Segment Routing Graph can't be loaded\n");
                return EXIT_FAILURE;
            }

            INFO("Segment Routing Graph succesfully loaded\n");
        }
        else if (opt.labelsOrId == LOAD_LABELS)
        {
            sr = SrGraph_load_with_label(opt.filename, opt.accuracy, opt.biDir);

            if (sr == NULL)
            {
                ERROR("The Segment Routing Graph can't be loaded\n");
                return EXIT_FAILURE;
            }

            INFO("Segment Routing Graph succesfully loaded\n");
        }
    }
    else if (opt.loadingMode == LOAD_SR_BIN)
    {
        sr = SrGraph_load_bin(opt.filename);
        if (sr == NULL)
        {
            ERROR("The Segment Routing Graph can't be loaded\n");
            return EXIT_FAILURE;
        }
        INFO("Segment Routing Graph succesfully loaded\n");
    }
    else if (opt.loadingMode == LOAD_TOPO_AREAS)
    {
        areas = Topology_load_multiple_areas(opt.filename, opt.accuracy, opt.biDir, opt.nb_areas);

        if (areas == NULL)
        {
            ERROR("Topology can't be load\n");
            return EXIT_FAILURE;
        }

        INFO("Areas succesfully loaded\n");

        sr_areas = calloc(opt.nb_areas, sizeof(SrGraph_t *));
        long int *times_tr = calloc(opt.nb_areas, sizeof(long int));
        for (int i = 0; i < opt.nb_areas; i++)
        {
            gettimeofday(&start, NULL);
            sr_areas[i] = SrGraph_create_from_topology_best_m2(areas[i]);
            gettimeofday(&stop, NULL);

            times_tr[i] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
        }

        main_display_area_sr_time_mean(times_tr, opt.nb_areas);

        free(times_tr);

        INFO("Areas SrGraph succesfully loaded\n");
    }
    else
    {
        ERROR("Please choose an available loading mode\n");
    }

    if (!opt.nb_areas)
    {
        ASSERT(sr, 1, 1);
    }
    else
    {
        ASSERT(sr_areas, 1, opt.nb_areas);
    }

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

    if (opt.saveSrGraphBin != NULL)
    {
        SrGraph_save_bin(sr, opt.saveSrGraphBin);
    }

    my_m1 maxSpread = 100;
    if (!opt.nb_areas)
    {
        maxSpread = SrGraph_get_max_spread(sr);
    }

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

    if (max_dict_size >= DICT_LIMIT)
    {
        ERROR("MaxSpread is too high. Please reduce the accuracy or your computer will probably die. "
              "You can also modifie the DICT_LIMIT value in include/params.h at line 44 if you are confident\n");
    }

    Dict_t **pf = NULL;
    Pfront_t **pfront = NULL;
    int maxIter = 0;

    if (opt.allNodes) {
        opt.allNodes = MIN(opt.allNodes, sr->nbNode);
    }

    if (opt.allNodes && !opt.nb_areas)
    {
        long int *times = malloc(opt.allNodes * sizeof(long int));
        int **iters = malloc(opt.allNodes * sizeof(int *));
        int *iterMax = malloc(opt.allNodes * sizeof(int));
        int **isFeasible = malloc(opt.allNodes * sizeof(int *));

        for (int i = 0; i < opt.allNodes; i++)
        {
            times[i] = 0;
            iters[i] = malloc(sr->nbNode * sizeof(int));
            isFeasible[i] = malloc(sr->nbNode * sizeof(int));
            pf = NULL;
            pfront = NULL;
            int iteri = 0;
            gettimeofday(&start, NULL);

            iteri = Best2cop(&pfront, &pf, sr, i, opt.cstr1, opt.cstr2, max_dict_size, opt.analyse, &iters[i]);

            gettimeofday(&stop, NULL);
            iterMax[i] = MAX(iteri, iterMax[i]);
            times[i] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
            //RESULTS("Iter max : %d\n", iterMax[i]);
           // struct segment_list ***sl = Segment_list_retreive_paths(pf, sr, iterMax[i], i);
			//main_display_distances(output, pf, iterMax[i], sr->nbNode, i, topo, sl);
			//segment_list_free(sl, iter, sr->nbNode);
            //printf("\r");

            if (opt.analyse)
            {
                maxIter = 10 * SEG_MAX;
            }
            else
            {
                maxIter = SEG_MAX;
            }

            for (int j = 0; j < sr->nbNode; j++)
            {
                if (iters[i][j] > SEG_MAX || iters[i][j] < 0)
                {
                    if (!hasapath(pfront, j))
                    {
                        isFeasible[i][j] = 0;
                    }
                    else
                    {
                        isFeasible[i][j] = 1;
                    }
                }
                else
                {
                    isFeasible[i][j] = 1;
                }
            }

            for (int j = 0; j <= maxIter; j++)
            {
                for (int k = 0; k < sr->nbNode; k++)
                {
                    Pfront_free(&pfront[j][k]);
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
            opt.src = LabelTable_get_id(topo->labels, opt.src_lab);
        }
        pf = NULL;
        pfront = NULL;
        int *itersSolo = malloc(sr->nbNode * sizeof(int));
        //printf("params\nsrc = %d\ncstr1 = %d\ncstr2 = %d\ndict size = %d\nmaxSpread = %d\n", opt.src, opt.cstr1, opt.cstr2, max_dict_size, maxSpread);
        gettimeofday(&start, NULL);

        int iter = Best2cop(&pfront, &pf, sr, opt.src, opt.cstr1, opt.cstr2, max_dict_size + 1, opt.analyse, &itersSolo);

        gettimeofday(&stop, NULL);
        long int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

        if (opt.analyse)
        {
            fprintf(output, "Execution stop after %d iterations\n", iter);
        }
        else
        {
            RESULTS("Execution takes %ld us\n", time);
        }

        if (opt.analyse)
        {
            maxIter = 10 * SEG_MAX;
        }
        else
        {
            maxIter = SEG_MAX + 1;
        }

        struct segment_list ***sl = Segment_list_retreive_paths(pf, sr, iter, opt.src);
        main_display_distances(output, pf, iter, sr->nbNode, opt.src, topo, sl);
        //Main_display_results()

        for (int j = 0; j < maxIter; j++)
        {
            for (int k = 0; k < sr->nbNode; k++)
            {
                Pfront_free(&pfront[j][k]);
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
    /* If the topology is decomposed into many areas */
    else if (opt.nb_areas > 0)
    {
        if (!opt.analyse && !opt.allNodes)
        {
            struct segment_list ****sl_areas = calloc(opt.nb_areas * 2, sizeof(struct segment_list ***));
            ASSERT(sl_areas, EXIT_FAILURE, opt.nb_areas * 2);

            long int *times_area = calloc(opt.nb_areas * 2, sizeof(long int));
            ASSERT(times_area, EXIT_FAILURE, opt.nb_areas * 2);

            compact_front **cf_area = calloc(opt.nb_areas * 2, sizeof(compact_front *));
            ASSERT(cf_area, EXIT_FAILURE, opt.nb_areas * 2);
            int area_src;
            int iter;
            int src = -1;

            for (int i = 0; i < opt.nb_areas; i++)
            {
                for (int id = 0; id < 2; id++)
                {
                    int index = (id)*opt.nb_areas + i;
                    pf = NULL;
                    pfront = NULL;
                    if (i)
                    {
                        area_src = Topology_search_abr_id(areas[i], 0, i, id);
                    }
                    else
                    {
                        area_src = Topology_search_abr_id(areas[i], 0, opt.nb_areas - 1, id);
                        if (src == -1)
                        {
                            src = area_src;
                        }
                    }

                    gettimeofday(&start, NULL);
                    /* first do the BEST2COP on each areas */
                    iter = Best2cop(&pfront, &pf, sr_areas[i], area_src, opt.cstr1, opt.cstr2, max_dict_size + 1, false, NULL);

                    /* then retreive the paths using the pareto front */
                    sl_areas[index] = Segment_list_retreive_paths(pf, sr_areas[i], iter, area_src);

#ifdef DEBUG_ALL
                    if (!id && i == opt.nb_areas - 1)
                    {
                        main_display_distances(output, pf, iter, sr_areas[i]->nbNode, area_src, areas[i], sl_areas[index]);
                    }
#endif

                    /* finally, tranform the tab into a list (emulate packet formation) */
                    cf_area[index] = compact_to_array_2D(pfront, pf, iter, sr_areas[i]->nbNode, sl_areas[index]);

                    gettimeofday(&stop, NULL);
                    times_area[index] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

                    maxIter = SEG_MAX + 1;

                    for (int j = 0; j < maxIter; j++)
                    {
                        for (int k = 0; k < sr_areas[i]->nbNode; k++)
                        {
                            Pfront_free(&pfront[j][k]);
                            Dict_free(&pf[j][k]);
                        }
                        free(pfront[j]);
                        free(pf[j]);
                    }

                    free(pfront);
                    free(pf);
                    segment_list_free(sl_areas[index], iter, sr_areas[i]->nbNode);
                }
            }
            main_display_area_time_mean(times_area, opt.nb_areas);
            long int tot_time_areas = 0;
            int size = 0;
            int index = 0;
            int index2 = 0;
            int area_src2 = 0;

            //#pragma omp parallel for
            for (int i = 1; i < opt.nb_areas; i++)
            {
                Dict_seglist_t **merged[2];
                Dict_seglist_t **final;
                for (int id = 0; id < 2; id++)
                {
                    merged[id] = NULL;
                    index = (id)*opt.nb_areas + i;
                    index2 = ((id + 1) % 2) * opt.nb_areas + i;
                    area_src = Topology_search_abr_id(areas[0], 0, i, id);
                    area_src2 = Topology_search_abr_id(areas[0], 0, i, (id + 1) % 2);

                    gettimeofday(&start, NULL);
                    merged[id] = cart(cf_area[0], cf_area[index],
                                      cf_area[index2], opt.cstr1, area_src, area_src2, sr_areas[0], areas[0], areas[i], src, opt.analyse);
                    gettimeofday(&stop, NULL);
                    size++;
                    //RESULTS("For index %d -> %ld us\n", index, (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
                    tot_time_areas += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
                }

                final = compact_pareto_front_ify(merged, cf_area[i]->nbNodes, opt.analyse);

#ifdef DEBUG_ALL

                for (int j = 0; j <= SEG_MAX; j++)
                {
                    for (int k = 0; k < cf_area[index]->nbNodes; k++)
                    {
                        for (int y = 0; y < final[j][k].size; y++)
                        {
                            if (final[j][k].paths[y] != INF)
                            {
                                fprintf(output, "%s %s %d %d %d", LabelTable_get_name(areas[0]->labels, Topology_search_abr_id(areas[0], 0, opt.nb_areas - 1, 0)),
                                        LabelTable_get_name(areas[i]->labels, k), j, y, final[j][k].paths[y]);
                                Segment_list_print(output, &final[j][k].seg_list[y], areas[0], areas[i]);
                                //Segment_list_print_id(output, &final[j][k].seg_list[y]);
                                fprintf(output, "\n");
                            }
                        }
                    }
                }
#endif
                if (opt.analyse)
                {
                    Segment_list_print_analyse(output, final, areas[i]->nbNode, SEG_MAX, opt.analyse, areas[i]);
                }

                for (int id = 0; id < 2; id++)
                {
                    index = (id)*opt.nb_areas + i;
                    for (int j = 0; j <= SEG_MAX; j++)
                    {
                        for (int k = 0; k < cf_area[index]->nbNodes; k++)
                        {
                            Dict_seglist_free(&merged[id][j][k]);
                        }
                        free(merged[id][j]);
                    }
                    free(merged[id]);
                }

                for (int j = 0; j <= SEG_MAX; j++)
                {
                    for (int k = 0; k < cf_area[i]->nbNodes; k++)
                    {
                        Dict_seglist_free(&final[j][k]);
                    }
                    free(final[j]);
                }
                free(final);
            }

            if (!opt.analyse)
            {
                RESULTS("Total computation (%d cartesian products) takes %ld us\n", size, tot_time_areas);
            }

            for (int i = 0; i < opt.nb_areas; i++)
            {
                for (int id = 1; id < 3; id++)
                {
                    index = (id - 1) * opt.nb_areas + i;
                    Compact_free(cf_area[index]);
                }
            }

            for (int i = 0; i < opt.nb_areas; i++)
            {
                Topology_free(areas[i]);
                SrGraph_free(sr_areas[i]);
            }

            free(cf_area);
            free(times_area);
            free(sl_areas);
        }
        else if (opt.analyse)
        {
            run_acc_cartesian_product(areas, sr_areas, output);
        }
        else if (opt.allNodes)
        {
            fprintf(output, "SRC_ABR MEAN_AREA SQUARE_AREA CART\n");
            for (int it1 = 1; it1 < opt.nb_areas; it1++)
            {
                for (int it2 = 0; it2 < 2; it2++)
                {
                    run_cart_on_specific_ABR(areas, sr_areas, output, it1, it2);
                }
            }

            for (int i = 0; i < opt.nb_areas; i++)
            {
                Topology_free(areas[i]);
                SrGraph_free(sr_areas[i]);
            }
        }
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
        Topology_free(topo);
        SrGraph_free(sr);
    }
    else
    {
        free(areas);
        free(sr_areas);
    }
    return 0;
}

void Main_display_results(FILE *output, Dict_t **dist, int nbNodes, __attribute__((unused)) Pfront_t **heap, int iter)
{
    for (int i = 0; i < nbNodes; i++)
    {
        for (int k = 0; k < iter; k++)
        {
            // for (ParetoFront_t* tmp = dist[k][i] ; tmp != NULL ; tmp = tmp->next) {
            //     fprintf(output, "%d %d %d %d\n", i, tmp->m1, tmp->m2, k);
            // }

            for (int j = 0; j < dist[k][i].size; j++)
            {
                if (dist[k][i].paths[j] != INF)
                {
                    fprintf(output, "%d %d %d %d\n", i, j, dist[k][i].paths[j], k);
                }
            }
        }
    }
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
                    fprintf(output, "%d %d %d\n", i, tmp->m1, tmp->m2);
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
            fprintf(output, "%d %d %d %ld %d\n", i, opt.cstr1, opt.nbThreads, tab[i], *tabIter[i]);
        }
    }
}

void Main_get_all_infos(struct Options *opt)
{
    printf("Enter your file name : \n");
    opt->filename = malloc(128);
    int scan = scanf("%s", opt->filename);
    if (scan == EOF)
    {
        return;
    }

    int get = 0;
    printf("Load as a topology file(1) or a Segment Routing Graph(2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF)
    {
        return;
    }
    opt->loadingMode = get - 1;

    printf("Are the node in the file identified by Labels (1) or IDs (2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF)
    {
        return;
    }
    opt->labelsOrId = get - 1;

    printf("Do you want to compute on only one node (1) or all (2) ?\n");
    scan = scanf("%d", &get);
    if (scan == EOF)
    {
        return;
    }
    opt->allNodes = get - 1;

    if (!opt->allNodes)
    {
        printf("Please enter the id of the source node\n");
        scan = scanf("%d", &get);
        if (scan == EOF)
        {
            return;
        }
        opt->src = get;
    }
}

void main_display_area_time_mean(long int *times, int nb_areas)
{
    long int tot, mean, square;
    tot = 0;
    for (int i = 1; i < nb_areas - 1; i++)
    {
        for (int id = 0; id < 2; id++)
        {
            int index = (id)*nb_areas + i;
            tot += times[index];
        }
    }

    mean = tot / ((nb_areas - 2) * 2);

    tot = 0;
    for (int i = 1; i < nb_areas - 1; i++)
    {
        for (int id = 0; id < 2; id++)
        {
            int index = (id)*nb_areas + i;
            tot += (mean - times[index]) * (mean - times[index]);
            //RESULTS("Value is %ld\n", times[index]);
        }
    }
    square = tot / ((nb_areas - 2) * 2);
    square = sqrt(square);

    RESULTS("Mean time for areas is %ld us\n", mean);
    RESULTS("Standard deviation (95%%) for areas is %ld us\n", 2 * square);
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
    for (int i = 0; i < iter; i++)
    {
        for (int j = 0; j < nbNodes; j++)
        {
            for (int k = 0; k < dist[i][j].size; k++)
            {
                if (dist[i][j].paths[k] != INF)
                {
                    //fprintf(out, "%s %s %d %d %d", LabelTable_get_name(topo->labels, src),
                            //LabelTable_get_name(topo->labels, j), i, k, dist[i][j].paths[k]);
                    //segment_list_invert(&sl[i][j][k]);
                    //Segment_list_print(out, &sl[i][j][k], topo, NULL);
                    //Segment_list_print_id(out, &sl[i][j][k]);
                    //fprintf(out, "\n");
                    fprintf(out, "%d %d %d %d\n", j, k, dist[i][j].paths[k], i);
                }
            }
        }
    }
}

int run_acc_cartesian_product(Topology_t **areas, SrGraph_t **sr_areas, FILE *output)
{
    // this structure holds the segment lists for all the
    struct segment_list ****sl_areas;
    compact_front **cf_area;
    int area_src;
    int iter;
    int src = -1;
    compact_front **cf_acc[2];
    Pfront_t **pfront;
    Dict_t **pf;
    Dict_seglist_t **merged[2];
    Dict_seglist_t ****merged_acc;
    Dict_seglist_t **final = NULL;
    Dict_seglist_t ***final_acc;
    int index = 0;
    int index2 = 0;
    int area_src2 = 0;

    cf_area = calloc(opt.nb_areas * 2, sizeof(compact_front *));
    ASSERT(cf_area, EXIT_FAILURE, opt.nb_areas * 2);

    sl_areas = calloc(opt.nb_areas * 2, sizeof(struct segment_list ***));
    ASSERT(sl_areas, EXIT_FAILURE, opt.nb_areas * 2);

    cf_acc[0] = calloc(opt.nb_areas, sizeof(compact_front **));
    cf_acc[1] = calloc(opt.nb_areas, sizeof(compact_front **));

    merged_acc = calloc(opt.nb_areas, sizeof(Dict_seglist_t ***));
    for (int i = 0; i < opt.nb_areas; i++)
    {
        merged_acc[i] = calloc(2, sizeof(Dict_seglist_t **));
    }

    final_acc = calloc(opt.nb_areas, sizeof(Dict_seglist_t **));

    INFO("Start Analysis computation\n");
    /***********************************************************************************
 ******************* Start of computation *****************************************
***********************************************************************************/

    for (int i = 0; i < opt.nb_areas; i++)
    {
        for (int id = 0; id < 2; id++)
        {
            int index = (id)*opt.nb_areas + i;
            pf = NULL;
            pfront = NULL;
            if (i)
            {
                // Search for the required ABR
                area_src = Topology_search_abr_id(areas[i], 0, i, id);
            }
            else
            {
                // Set the source to city-ABR0.area.id
                area_src = Topology_search_abr_id(areas[i], 0, opt.nb_areas - 1, id);
                if (src == -1)
                {
                    src = area_src;
                }
            }

            /* first do the BEST2COP on each areas */
            //INFO("Start best2cop\n");
            iter = Best2cop(&pfront, &pf, sr_areas[i], area_src, opt.cstr1, opt.cstr2, 1001, opt.analyse, NULL);
            //INFO("End BEST2cop\n");
            /* then retreive the paths using the pareto front */
            //INFO("Start Retreive\n");
            sl_areas[index] = Segment_list_retreive_paths(pf, sr_areas[i], iter, area_src);
            //INFO("End Retreive\n");

            /* finally, tranform the tab into a list (emulate packet formation) */
            //INFO("start conversion\n");
            cf_area[index] = compact_to_array_2D(pfront, pf, iter, sr_areas[i]->nbNode, sl_areas[index]);
            //INFO("End conversion\n");

            for (int j = 0; j <= SEG_MULT * SEG_MAX; j++)
            {
                for (int k = 0; k < sr_areas[i]->nbNode; k++)
                {
                    Pfront_free(&pfront[j][k]);
                    Dict_free(&pf[j][k]);
                }
                free(pfront[j]);
                free(pf[j]);
            }
            free(pfront);
            free(pf);
            segment_list_free(sl_areas[index], iter, sr_areas[i]->nbNode);
        }
    }

    INFO("Start cartésian Product\n");
    /******* NOW LET US DO THE CARTESIANS PRODUCTS *************/

    for (int i = 1; i < opt.nb_areas - 1; i++)
    {
        // for the first ABR of area 0.
        for (int id = 0; id < 2; id++)
        {
            merged[id] = NULL;
            index = (id)*opt.nb_areas + i;
            index2 = ((id + 1) % 2) * opt.nb_areas + i;

            // search the first ABR id
            area_src = Topology_search_abr_id(areas[0], 0, i, id);

            // Search the other ABR id
            area_src2 = Topology_search_abr_id(areas[0], 0, i, (id + 1) % 2);

            // Search for the source
            src = Topology_search_abr_id(areas[0], 0, opt.nb_areas - 1, 0);

            // Then do the cartesian product
            merged[id] = cart(cf_area[0], cf_area[index],
                              cf_area[index2], opt.cstr1, area_src, area_src2, sr_areas[0], areas[0], areas[i], src, opt.analyse);
        }

        // Transform the cartesian products results into a pareto front
        final = compact_pareto_front_ify(merged, cf_area[i]->nbNodes, opt.analyse);
        cf_acc[0][i] = dict_seglist_to_compact(NULL, final, SEG_MULT * SEG_MAX, areas[i]->nbNode);

        // Macro to free 3D arrays
        FREE_3D(2, (SEG_MULT * SEG_MAX + 1), cf_area[i]->nbNodes, merged, Dict_seglist_free, mer);

        FREE_2D(SEG_MULT * SEG_MAX, cf_area[i]->nbNodes, final, Dict_seglist_free, fin);
        free(final);

        for (int id = 0; id < 2; id++)
        {
            merged[id] = NULL;
            index = (id)*opt.nb_areas + i;
            index2 = ((id + 1) % 2) * opt.nb_areas + i;

            // search the first ABR id
            area_src = Topology_search_abr_id(areas[0], 0, i, id);

            // Search the other ABR id
            area_src2 = Topology_search_abr_id(areas[0], 0, i, (id + 1) % 2);

            // Search for the source
            src = Topology_search_abr_id(areas[0], 0, opt.nb_areas - 1, 1);

            // Then do the cartesian product
            merged[id] = cart(cf_area[opt.nb_areas], cf_area[index],
                              cf_area[index2], opt.cstr1, area_src, area_src2, sr_areas[0], areas[0], areas[i], src, opt.analyse);
        }

        // Transform the cartesian products results into a pareto front
        final = compact_pareto_front_ify(merged, cf_area[i]->nbNodes, opt.analyse);
        cf_acc[1][i] = dict_seglist_to_compact(NULL, final, SEG_MULT * SEG_MAX, areas[i]->nbNode);

        FREE_3D(2, (SEG_MULT * SEG_MAX + 1), cf_area[i]->nbNodes, merged, Dict_seglist_free, mer);

        FREE_2D(SEG_MULT * SEG_MAX, cf_area[i]->nbNodes, final, Dict_seglist_free, fin);
        free(final);
    }
    struct segment_list ***sl_acc;
    compact_front *cf_acc_acc;

    // Find the required ACC router.
    src = Topology_search_acc_id(areas[opt.nb_areas - 1], opt.nb_areas - 1, 1, 1);

    // Do the BEST2COP on the ACC router area
    iter = Best2cop(&pfront, &pf, sr_areas[opt.nb_areas - 1], src, opt.cstr1, opt.cstr2, 1001, false, NULL);

    /* then retreive the paths using the pareto front */
    sl_acc = Segment_list_retreive_paths(pf, sr_areas[opt.nb_areas - 1], iter, src);

    /* finally, tranform the tab     into a list (emulate packet formation) */
    cf_acc_acc = compact_to_array_2D(pfront, pf, iter, sr_areas[opt.nb_areas - 1]->nbNode, sl_acc);

    for (int i = 1; i < opt.nb_areas - 1; i++)
    {
        for (int id = 0; id < 2; id++)
        {

            // search the first ABR of the area.
            area_src = Topology_search_abr_id(areas[opt.nb_areas - 1], 0, opt.nb_areas - 1, id);

            // Serach the second ABR of the area
            area_src2 = Topology_search_abr_id(areas[opt.nb_areas - 1], 0, opt.nb_areas - 1, (id + 1) % 2);

            // Then do the cartesian product toward each dest of each areas.
            merged_acc[i][id] = cart(cf_acc_acc, cf_acc[id][i], cf_acc[(id + 1) % 2][i], opt.cstr1,
                                     area_src, area_src2, sr_areas[opt.nb_areas - 1], areas[opt.nb_areas - 1], areas[i], src, opt.analyse);

#ifdef DEBUG_ALL
            for (int j = 0; j <= SEG_MAX; j++)
            {
                for (int k = 0; k < areas[i]->nbNode; k++)
                {
                    for (int y = 0; y < merged_acc[i][id][j][k].size; y++)
                    {
                        if (merged_acc[i][id][j][k].paths[y] != INF)
                        {
                            fprintf(output, "%s %s %d %d %d", LabelTable_get_name(areas[opt.nb_areas - 1]->labels, src),
                                    LabelTable_get_name(areas[i]->labels, k), j, y, merged_acc[i][id][j][k].paths[y]);
                            Segment_list_print(output, &merged_acc[i][id][j][k].seg_list[y], areas[opt.nb_areas - 1], areas[i]);
                            //Segment_list_print_id(output, &final[j][k].seg_list[y]);
                            fprintf(output, "\n");
                        }
                    }
                }
            }
#else
            UNUSED(output);
#endif
        }
    }

    float nb_dist = 0;
    float nb_nodes = 0;

    for (int i = 1; i < opt.nb_areas - 1; i++)
    {

        // Compute the Segment list from ACC to other destinations.
        final_acc[i] = compact_pareto_front_ify_3D(merged_acc[i], areas[i]->nbNode, opt.analyse);

        // Display the results using the right analysis type.
        nb_dist += Segment_list_print_analyse(output, final_acc[i], areas[i]->nbNode, SEG_MULT * SEG_MAX, opt.analyse, areas[i]);
        nb_nodes += areas[i]->nbNode;

        FREE_3D(2, (SEG_MULT * SEG_MAX + 1), cf_area[i]->nbNodes, merged_acc[i], Dict_seglist_free, mer);

        FREE_2D(SEG_MULT * SEG_MAX, cf_area[i]->nbNodes, final_acc[i], Dict_seglist_free, fin);
        free(final_acc[i]);
        free(merged_acc[i]);
    }

    if (nb_dist)
    {
        RESULTS("Mean dist to node : %f (%f / %f)\n", nb_dist / nb_nodes, nb_dist, nb_nodes);
    }

    free(merged_acc);
    free(final_acc);

    for (int i = 0; i < opt.nb_areas - 1; i++)
    {
        for (int id = 1; id < 3; id++)
        {
            index = (id - 1) * opt.nb_areas + i;
            Compact_free(cf_area[index]);
        }
    }
    for (int i = 0; i < opt.nb_areas; i++)
    {
        Topology_free(areas[i]);
        SrGraph_free(sr_areas[i]);
    }
    free(cf_area);
    free(sl_areas);

    return EXIT_SUCCESS;
}

void main_display_results_for_all_abrs(long int *times, int nb_areas, long int tot_time, char *abr, FILE *output, int abr_area)
{
    long int tot, mean, square;
    tot = 0;
    for (int i = 1; i < nb_areas; i++)
    {
        if (i == abr_area)
        {
            continue;
        }
        for (int id = 0; id < 2; id++)
        {
            int index = (id)*nb_areas + i;
            tot += times[index];
        }
    }

    mean = tot / ((nb_areas - 2) * 2);

    tot = 0;
    for (int i = 1; i < nb_areas; i++)
    {
        if (i == abr_area)
        {
            continue;
        }
        for (int id = 0; id < 2; id++)
        {
            int index = (id)*nb_areas + i;
            tot += (mean - times[index]) * (mean - times[index]);
            //RESULTS("Value is %ld\n", times[index]);
        }
    }
    square = tot / ((nb_areas - 2) * 2);
    square = sqrt(square);

    fprintf(output, "%s %ld %ld %ld\n", abr, mean, 2 * square, tot_time);
}

void run_cart_on_specific_ABR(Topology_t **areas, SrGraph_t **sr_areas, FILE *output, int abr_area, int abr_id)
{
    struct timeval stop, start;
    Pfront_t **pfront;
    Dict_t **pf;
    int max_dict_size = 1000;
    int maxIter = SEG_MAX + 1;
    struct segment_list ****sl_areas = calloc(opt.nb_areas * 2, sizeof(struct segment_list ***));
    ASSERT(sl_areas, , opt.nb_areas * 2);

    long int *times_area = calloc(opt.nb_areas * 2, sizeof(long int));
    ASSERT(times_area, , opt.nb_areas * 2);

    compact_front **cf_area = calloc(opt.nb_areas * 2, sizeof(compact_front *));
    ASSERT(cf_area, , opt.nb_areas * 2);
    int area_src;
    int iter;
    int src = -1;

    for (int i = 1; i < opt.nb_areas; i++)
    {
        for (int id = 0; id < 2; id++)
        {
            int index = (id)*opt.nb_areas + i;
            pf = NULL;
            pfront = NULL;
            if (i != abr_area)
            {
                area_src = Topology_search_abr_id(areas[i], 0, i, id);

                gettimeofday(&start, NULL);
                /* first do the BEST2COP on each areas */
                iter = Best2cop(&pfront, &pf, sr_areas[i], area_src, opt.cstr1, opt.cstr2, max_dict_size + 1, false, NULL);

                /* then retreive the paths using the pareto front */
                sl_areas[index] = Segment_list_retreive_paths(pf, sr_areas[i], iter, area_src);

#ifdef DEBUG_ALL
                if (!id && i == opt.nb_areas - 1)
                {
                    main_display_distances(output, pf, iter, sr_areas[i]->nbNode, area_src, areas[i], sl_areas[index]);
                }
#endif

                /* finally, tranform the tab into a list (emulate packet formation) */
                cf_area[index] = compact_to_array_2D(pfront, pf, iter, sr_areas[i]->nbNode, sl_areas[index]);

                gettimeofday(&stop, NULL);
                times_area[index] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

                for (int j = 0; j < maxIter; j++)
                {
                    for (int k = 0; k < sr_areas[i]->nbNode; k++)
                    {
                        Pfront_free(&pfront[j][k]);
                        Dict_free(&pf[j][k]);
                    }
                    free(pfront[j]);
                    free(pf[j]);
                }

                free(pfront);
                free(pf);
                segment_list_free(sl_areas[index], iter, sr_areas[i]->nbNode);
            }
            else
            {
                area_src = Topology_search_abr_id(areas[0], 0, abr_area, id);
                if (src == -1)
                {
                    src = area_src;
                }

                gettimeofday(&start, NULL);
                /* first do the BEST2COP on each areas */
                iter = Best2cop(&pfront, &pf, sr_areas[0], area_src, opt.cstr1, opt.cstr2, max_dict_size + 1, false, NULL);

                /* then retreive the paths using the pareto front */
                sl_areas[index] = Segment_list_retreive_paths(pf, sr_areas[0], iter, area_src);

#ifdef DEBUG_ALL
                if (!id && i == opt.nb_areas - 1)
                {
                    main_display_distances(output, pf, iter, sr_areas[i]->nbNode, area_src, areas[i], sl_areas[index]);
                }
#endif

                /* finally, tranform the tab into a list (emulate packet formation) */
                cf_area[index] = compact_to_array_2D(pfront, pf, iter, sr_areas[0]->nbNode, sl_areas[index]);

                gettimeofday(&stop, NULL);
                times_area[index] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

                for (int j = 0; j < maxIter; j++)
                {
                    for (int k = 0; k < sr_areas[0]->nbNode; k++)
                    {
                        Pfront_free(&pfront[j][k]);
                        Dict_free(&pf[j][k]);
                    }
                    free(pfront[j]);
                    free(pf[j]);
                }

                free(pfront);
                free(pf);
                segment_list_free(sl_areas[index], iter, sr_areas[0]->nbNode);
            }
        }
    }
    //main_display_area_time_mean(times_area, opt.nb_areas);
    long int tot_time_areas = 0;
    int size = 0;
    int index = 0;
    int index2 = 0;
    int area_src2 = 0;
    int index_computed = abr_id * opt.nb_areas + abr_area;

    //#pragma omp parallel for
    for (int i = 1; i < opt.nb_areas; i++)
    {
        if (i == abr_area)
        {
            continue;
        }

        Dict_seglist_t **merged[2];
        Dict_seglist_t **final;

        for (int id = 0; id < 2; id++)
        {
            merged[id] = NULL;
            index = (id)*opt.nb_areas + i;
            index2 = ((id + 1) % 2) * opt.nb_areas + i;
            area_src = Topology_search_abr_id(areas[0], 0, i, id);
            area_src2 = Topology_search_abr_id(areas[0], 0, i, (id + 1) % 2);

            gettimeofday(&start, NULL);
            merged[id] = cart(cf_area[index_computed], cf_area[index],
                              cf_area[index2], opt.cstr1, area_src, area_src2, sr_areas[0], areas[0], areas[i], src, opt.analyse);
            gettimeofday(&stop, NULL);
            size++;
            //RESULTS("For index %d -> %ld us\n", index, (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
            tot_time_areas += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
        }

        final = compact_pareto_front_ify(merged, cf_area[i]->nbNodes, opt.analyse);

#ifdef DEBUG_ALL

        for (int j = 0; j <= SEG_MAX; j++)
        {
            for (int k = 0; k < cf_area[index]->nbNodes; k++)
            {
                for (int y = 0; y < final[j][k].size; y++)
                {
                    if (final[j][k].paths[y] != INF)
                    {
                        fprintf(output, "%s %s %d %d %d", LabelTable_get_name(areas[0]->labels, Topology_search_abr_id(areas[0], 0, opt.nb_areas - 1, 0)),
                                LabelTable_get_name(areas[i]->labels, k), j, y, final[j][k].paths[y]);
                        Segment_list_print(output, &final[j][k].seg_list[y], areas[0], areas[i]);
                        //Segment_list_print_id(output, &final[j][k].seg_list[y]);
                        fprintf(output, "\n");
                    }
                }
            }
        }
#endif

        for (int id = 0; id < 2; id++)
        {
            index = (id)*opt.nb_areas + i;
            for (int j = 0; j <= SEG_MAX; j++)
            {
                for (int k = 0; k < cf_area[index]->nbNodes; k++)
                {
                    Dict_seglist_free(&merged[id][j][k]);
                }
                free(merged[id][j]);
            }
            free(merged[id]);
        }

        for (int j = 0; j <= SEG_MAX; j++)
        {
            for (int k = 0; k < cf_area[i]->nbNodes; k++)
            {
                Dict_seglist_free(&final[j][k]);
            }
            free(final[j]);
        }
        free(final);
    }

    if (!opt.analyse)
    {
        //RESULTS("Total computation (%d cartesian products) takes %ld us\n", size, tot_time_areas);
    }

    main_display_results_for_all_abrs(times_area, opt.nb_areas, tot_time_areas,
                                      LabelTable_get_name(areas[0]->labels, Topology_search_abr_id(areas[0], 0,
                                                                                                   abr_area, abr_id)),
                                      output, abr_area);

    for (int i = 1; i < opt.nb_areas; i++)
    {
        for (int id = 1; id < 3; id++)
        {
            index = (id - 1) * opt.nb_areas + i;
            Compact_free(cf_area[index]);
        }
    }

    free(cf_area);
    free(times_area);
    free(sl_areas);
}
