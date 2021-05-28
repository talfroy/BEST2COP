#include "../include/SrGraph.h"
#include "../include/Option.h"


void my_print_progress(int percentage)
{
    int lpad = percentage * PBWIDTH / 100;
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", percentage, lpad, PBSTR, rpad, "");
    fflush(stdout);
}



SrGraph_t* SrGraph_init(int nbNodes)
{
    SrGraph_t* graph = malloc(sizeof(SrGraph_t));
    ASSERT(graph, NULL);

    graph->nbNode = nbNodes;

    graph->pred = malloc(nbNodes * sizeof(Edge_t**));
    ASSERT(graph->pred, NULL);

    // graph->succ = malloc(nbNodes * sizeof(Edge_t**));
    // ASSERT(graph->succ, NULL);

    for (int i = 0 ; i < nbNodes ; i++) {
        // graph->succ[i] = malloc(nbNodes * sizeof(Edge_t*));
        // ASSERT(graph->succ[i], NULL);

        graph->pred[i] = malloc(nbNodes * sizeof(Edge_t*));
        ASSERT(graph->pred[i], NULL);
        for (int j = 0 ; j < nbNodes ; j++) {
            // graph->succ[i][j] = NULL;
            graph->pred[i][j] = NULL;
        }
    }

    graph->m1dists = malloc(nbNodes * sizeof(my_m1*));
    ASSERT(graph->m1dists, NULL);

    graph->m2dists = malloc(nbNodes * sizeof(my_m2*));
    ASSERT(graph->m2dists, NULL);

    for (int i = 0 ; i < nbNodes ; i++) {

        graph->m2dists[i] = malloc(nbNodes * sizeof(my_m2));
        ASSERT(graph->m2dists[i], NULL);

        graph->m1dists[i] = malloc(nbNodes * sizeof(my_m1));
        ASSERT(graph->m1dists[i], NULL);
    }

    return graph;
}


void SrGraph_free(SrGraph_t* graph)
{
    for (int i = 0 ; i < graph->nbNode ; i++) {
        free(graph->m2dists[i]);
        free(graph->m1dists[i]);

        for (int j = 0 ; j < graph->nbNode ; j++) {
            Edge_free(graph->pred[i][j]);
            //Edge_free(graph->succ[i][j]);
        }

        free(graph->pred[i]);
        //free(graph->succ[i]);
    }

    free(graph->m2dists);
    free(graph->m1dists);
    free(graph->pred);
    //free(graph->succ);

    free(graph);
}




SrGraph_t* SrGraph_create_from_topology_best_m2(Topology_t* topo)
{
    SrGraph_t* graph = SrGraph_init(topo->nbNode);

    if (graph == NULL) {
        ERROR("Segment Routing Graph can't be initialized\n");
        return NULL;
    }

    //#pragma omp parallel for
    int last_per = 0;
    for (int i = 0 ; i < topo->nbNode ; i++) {
        
        //printf("Computing dijkstra for node %d\n", i);
        dikjstra_best_m2(&graph->pred, topo->succ, i,
                    &(graph->m1dists[i]), &(graph->m2dists[i]), topo->nbNode);

        if (opt.pretty) {
            if (((i + 1) * 100 / topo->nbNode) > last_per) {
                my_print_progress((i + 1) * 100 / topo->nbNode);
                last_per = (i + 1) * 100 / topo->nbNode;
                if (last_per == 100) {
                    printf("\n");
                }
            }
        }
    }

    for (int i = 0 ; i < topo->nbNode ; i++) {
        for (Llist_t* tmp = topo->succ[i] ; tmp != NULL ; tmp = tmp->next) {
            my_m1 m1 = tmp->infos.m1;
            my_m2 m2 = tmp->infos.m2;
            int dst = tmp->infos.edgeDst;
            if (m1 < graph->m1dists[i][dst]) {
                //graph->succ[i][dst] = Edge_add(graph->succ[i][dst], m1, m2);
                graph->pred[i][dst] = Edge_add(graph->pred[i][dst], m1, m2);
            }
        }
    }


    return graph;
}

void SrGraph_check_m1(SrGraph_t*sr)
{
    int nbEdge = 0;
    for (int i = 0 ; i < sr->nbNode ; i++) {
        for (int j = 0 ; j < sr->nbNode ; j++) {
            nbEdge = 0;
            for (Edge_t* tmp = sr->pred[i][j] ; tmp != NULL ; tmp = tmp->next) {
                nbEdge++;
                if (nbEdge > 1) {
                    printf("Aie coup dur pour guillaume\n");
                }
                if (tmp->m1 > 10000 || tmp->m1 < 0) {
                    printf("La c'est la merde : %d\n", tmp->m1);
                }
            }
        }
    }
}


SrGraph_t* SrGraph_create_from_topology_best_m1(Topology_t* topo)
{
    SrGraph_t* graph = SrGraph_init(topo->nbNode);

    if (graph == NULL) {
        ERROR("Segment Routing Graph can't be initialized\n");
        return NULL;
    }

    #pragma omp parallel for
    for (int i = 0 ; i < topo->nbNode ; i++) {
        dikjstra_best_m1(&graph->pred, topo->succ, i,
                    &(graph->m1dists[i]), &(graph->m2dists[i]), topo->nbNode);
    }

    for (int i = 0 ; i < topo->nbNode ; i++) {
        for (Llist_t* tmp = topo->succ[i] ; tmp != NULL ; tmp = tmp->next) {
            my_m1 m1 = tmp->infos.m1;
            my_m2 m2 = tmp->infos.m2;
            int dst = tmp->infos.edgeDst;
            if (m1 < graph->m1dists[i][dst]) {
                //graph->succ[i][dst] = Edge_add(graph->succ[i][dst], m1, m2);
                graph->pred[i][dst] = Edge_add(graph->pred[i][dst], m1, m2);
            }
        }
    }


    return graph;
}

SrGraph_t* SrGraph_merge_sr_graph(SrGraph_t* best_m1, SrGraph_t* best_m2, int size)
{
    SrGraph_t* flex = SrGraph_init(size);

    int nbNode = size;
    for (int src = 0 ; src < nbNode ; src++) {
        for (int dst = 0 ; dst < nbNode ; dst++) {
            if (src == dst) {
                continue;
            }
            //flex->succ[src][dst] = Edge_merge_flex(best_m1->succ[src][dst], best_m2->succ[src][dst]);
            flex->pred[dst][src] = Edge_merge_flex(best_m1->pred[dst][src], best_m2->pred[dst][src]);
        }
    }
    return flex;
}


SrGraph_t* SrGraph_create_flex_algo(Topology_t* topo)
{
    if (topo == NULL) {
        ERROR("Can't compute with an empty topology\n");
        return NULL;
    }
    SrGraph_t* best_m1 = SrGraph_create_from_topology_best_m1(topo);
    SrGraph_t* best_m2 = SrGraph_create_from_topology_best_m2(topo);
    SrGraph_t* flex = SrGraph_merge_sr_graph(best_m1, best_m2, topo->nbNode);

    SrGraph_free(best_m1);
    SrGraph_free(best_m2);

    return flex;
}


SrGraph_t* SrGraph_add_adjacencies(SrGraph_t* graph, Topology_t* topo)
{
    for (int i = 0 ; i < graph->nbNode ; i++) {
        for (Llist_t* edge = topo->pred[i] ; edge != NULL ; edge = edge->next) {
            my_m1 m1 = edge->infos.m1;
            my_m2 m2 = edge->infos.m2;
            int dst = edge->infos.edgeDst;

            if (graph->pred[i][dst] != NULL) {

                if (graph->pred[i][dst]->next == NULL) {
                    if (m1 < graph->pred[i][dst]->m1) {
                        graph->pred[i][dst] = Edge_add(graph->pred[i][dst], m1, m2);
                    }
                } else {

                    if (m1 < graph->pred[i][dst]->m1 && m2 < graph->pred[i][dst]->next->m2) {
                        graph->pred[i][dst] = Edge_add(graph->pred[i][dst], m1, m2);
                    }
                }
            }
        }
    }

    return graph;
}


SrGraph_t* SrGraph_load_with_id(char* filename, int nbNodes, int accuracy, char bi_dir)
{
    SrGraph_t* sr = SrGraph_init(nbNodes);

    if (sr == NULL) {
        ERROR("Segment Routing Graph can't be initialized\n");
        return NULL;
    }

    sr->nbNode = 0;

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        ERROR("File %s can't be found in the curent directory\n", filename);
        return NULL;
    }

    ASSERT(file, NULL);

    int src, dst;
    my_m2 m2;
    char line[1024];
    double m1;
    int nbLine = 1;

    while (fgets(line, 1024,  file))
    {
        if (sscanf(line, "%d %d %lf %d\n", &src, &dst, &m1, &m2)) {
            m1 *= my_pow(10, accuracy);
            //sr->succ[src][dst] = Edge_add(sr->succ[src][dst], m1, m2);
            sr->pred[dst][src] = Edge_add(sr->pred[dst][src], m1, m2);
            sr->nbNode = MAX(sr->nbNode, src + 1);
            sr->nbNode = MAX(sr->nbNode, dst + 1);
            if (bi_dir) {
                //sr->succ[dst][src] = Edge_add(sr->succ[dst][src], m1, m2);
                sr->pred[src][dst] = Edge_add(sr->pred[src][dst], m1, m2);
            }
        } else {
            ERROR("Can't load line %d : your file might not have the good format : \n\t[source_node]  [destination_node]  [delay]  [IGP_weight]\n", nbLine);
            SrGraph_free(sr);
            return NULL;
        }
        nbLine++;
    }

    fclose(file);
    return sr;
}


SrGraph_t* SrGraph_load_with_label(char* filename, int accuracy, char bi_dir)
{
    LabelTable_t labels;
    LabelTable_init(&labels);

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        ERROR("File %s can't be found in the current directory\n", filename);
        return NULL;
    }


    char srcLabel[128], dstLabel[128];
    my_m2 m2;
    char line[1024];
    double m1;
    int src, dst;
    int nbNode = 0;
    int nbLine = 1;

    while (fgets(line, 1024,  file))
    {
        if (sscanf(line, "%s %s %lf %d\n", &srcLabel[0], &dstLabel[0], &m1, &m2) == 4) {
            src = LabelTable_add_node(&labels, srcLabel);
            dst = LabelTable_add_node(&labels, dstLabel);
            nbNode = MAX(nbNode, labels.nextNodeId);
        } else {
            ERROR("Can't load line %d : your file might not have the good format : \n\t[source_node]  [destination_node]  [delay]  [IGP_weight]\n", nbLine);
            return NULL;
        }
        nbLine++;
    }
    fclose(file);

    LabelTable_sort(&labels);

    SrGraph_t* sr = SrGraph_init(nbNode);

    if (sr == NULL) {
        ERROR("Segment Routing Graph can't be initialized\n");
        return NULL;
    }

    file = fopen(filename, "r");

    if (file == NULL) {
        ERROR("File %s can't be found in the current directory\n", filename);
        return NULL;
    }

    nbLine = 1;


    while (fgets(line, 1024,  file))
    {
        if (sscanf(line, "%s %s %lf %d\n", &srcLabel[0], &dstLabel[0], &m1, &m2) == 4) {
            src = LabelTable_get_id(&labels, srcLabel);
            dst = LabelTable_get_id(&labels, dstLabel);

            m1 *= my_pow(10, accuracy);
            //sr->succ[src][dst] = Edge_add(sr->succ[src][dst], m1, m2);
            sr->pred[dst][src] = Edge_add(sr->pred[dst][src], m1, m2);
            if (bi_dir) {
                //sr->succ[dst][src] = Edge_add(sr->succ[dst][src], m1, m2);
                sr->pred[src][dst] = Edge_add(sr->pred[src][dst], m1, m2);
            }
        } else {
            ERROR("Can't load line %d : your file might not have the good format : \n\t[source_node]  [destination_node]  [delay]  [IGP_weight]\n", nbLine);
            SrGraph_free(sr);
            return NULL;
        }
        nbLine++;
    }

    fclose(file);
    LabelTable_free(&labels);

    return sr;
}


SrGraph_t* SrGraph_create_crash_test(int nbNode, int nbPlinks)
{
    SrGraph_t* graph = SrGraph_init(nbNode);

    if (graph == NULL) {
        ERROR("Segment Routing Graph can't be initialized\n");
    }

    for (int i = 0 ; i < nbNode ; i++) {
        for (int j = 0 ; j < nbNode ; j++) {
            if (i == j) {
                continue;
            }
            my_m1 m1 = 1;
            my_m2 m2 = 1;
            for (int k = 0 ; k < nbPlinks ; k++) {
                //graph->succ[i][j] = Edge_new(graph->succ[i][j], m1, m2);
                graph->pred[j][i] = Edge_new(graph->pred[j][i], m1, m2);
            }
        }
    }
    return graph;
}


int rand_c_d(int a, int b)
{
    return rand()%(b - a) + a;
}


SrGraph_t* SrGraph_create_random_topo(int nbNode, int maxSpread)
{
    srand(time(NULL));
    SrGraph_t* sr = SrGraph_init(nbNode);

    if (sr == NULL) {
        ERROR("Segment Routing Graph can't be initialized\n");
    }

    for (int i = 0 ; i < nbNode ; i++) {
        for (int j = 0 ; j < nbNode ; j++) {
            if (i == j) {
                continue;
            }

            for (int k = 0 ; k < 2 ; k++) {
                my_m1 m1 = rand_c_d(1, maxSpread);
                my_m2 m2 = rand_c_d(1, INT_MAX / 10 - 1);
                //sr->succ[i][j] = Edge_new(sr->succ[i][j], m1, m2);
                sr->pred[j][i] = Edge_new(sr->pred[j][i], m1, m2);
            }
        }
    }

    return sr;
}



void SrGraph_print_in_file(SrGraph_t* sr, FILE* output)
{
    for (int i = 0 ; i < sr->nbNode ; i++) {
        for (int j = 0 ; j < sr->nbNode ; j++) {
            if (i == j) {
                continue;
            }
            for (Edge_t* edge = sr->pred[i][j] ; edge != NULL ; edge = edge->next) {
                fprintf(output, "%d %d %d %d\n", i, j, edge->m1, edge->m2);
            }
            // fprintf(output, "%d -> %d : ", i, j);
            // Edge_print_list(sr->succ[i][j], output);
        }
    }
}


my_m1 SrGraph_get_max_spread(SrGraph_t* sr)
{
    my_m1 max = 0;
    my_m1 minM1 = INF;
    my_m2 minM2 = INF;
    my_m2 maxM2 = 0;
    int nbzero = 0;
    int nbEdge = 0;
    for (int i = 0 ; i < sr->nbNode ; i++) {
        for (int j = 0 ; j < sr->nbNode ; j++) {
            //printf("debut (%d ; %d)\n", i, j);
            for (Edge_t* tmp = sr->pred[i][j] ; tmp != NULL ; tmp = tmp->next) {
                // if (tmp->m1 == INF) {
                //     return -1;
                // }
                minM1 = MIN(minM1, tmp->m1);
                minM2 = MIN(minM2, tmp->m2);
                maxM2 = MAX(maxM2, tmp->m2);
                max = MAX(max, tmp->m1);
                if (!tmp->m1) {
                    nbzero++;
                }
                nbEdge++;
            }
            //printf("fin (%d ; %d)\n", i, j);
        }
    }

    INFO("There are %d adjacencies\n", nbEdge- (sr->nbNode) * (sr->nbNode - 1));
    RESULTS("Max delay : %d\n", max);
    RESULTS("Min delay : %d\n", minM1);
    RESULTS("Max igp : %d\n", maxM2);
    RESULTS("Min igp : %d\n", minM2);
    RESULTS("NB edges with 0 : %d\n", nbzero);
    return max;
}


bool SrGraph_is_connex(SrGraph_t* sr)
{
    for (int i = 0 ; i < sr->nbNode ; i++) {
        for (int j = 0 ; j < sr->nbNode ; j++) {
            //printf("debut (%d ; %d)\n", i, j);
            for (Edge_t* tmp = sr->pred[i][j] ; tmp != NULL ; tmp = tmp->next) {
                if (tmp->m1 == INF) {
                    return false;
                }
            }
            //printf("fin (%d ; %d)\n", i, j);
        }
    }
    return true;
}



/*void SrGraph_prune(SrGraph_t* graph, int src, my_m1 cstrIgp, my_m2 cstrDelay)
{
    my_m1 minIgp = INF;
    my_m2 minDelay = INF;
    edge edgeMinIgp;
    edge edgeMinDelay;
    edgeMinDelay.src = 0;
    edgeMinDelay.dst = 0;
    edgeMinIgp.src = 0;
    edgeMinIgp.dst = 0;
    edge edgeToRemove[graph->nbNode * graph->nbNode];
    int nbEdge = 0;
    int nextEdge = 0;

    for (int i = 0 ; i < graph->nbNode ; i++) {
        for (int j = 0 ; j < graph->nbNode ; j++) {
            for (Edge_t* tmp = graph->succ[i][j] ; tmp != NULL ; tmp = tmp->next) {
                nbEdge++;
                if (tmp->m1 < minIgp) {
                    minIgp = tmp->m1;
                    edgeMinIgp.src = i;
                    edgeMinIgp.dst = j;
                }

                if (tmp->m2 < minDelay) {
                    minDelay = tmp->m2;
                    edgeMinDelay.src = i;
                    edgeMinDelay.dst = j;
                }
            }
        }
    }

    printf("%d edges\n", nbEdge);
    printf("min igp : %d edge : (%d ; %d)\n", minIgp, edgeMinIgp.src, edgeMinIgp.dst);
    printf("min delay : %d edge : (%d ; %d)\n", minDelay, edgeMinDelay.src, edgeMinDelay.dst);

    nbEdge = 0;

    for (int i = 0 ; i < graph->nbNode ; i++) {
        for (int j = 0 ; j < graph->nbNode ; j++) {
            for (Edge_t* tmp = graph->succ[i][j] ; tmp != NULL ; tmp = tmp->next) {

                if (i != src && j != src) {

                    if ((tmp->m1 + minIgp >= cstrIgp) && !((edgeMinDelay.src == i) && (edgeMinDelay.dst == j))) {
                        edgeToRemove[nextEdge].src = i;
                        edgeToRemove[nextEdge].dst = j;
                        nextEdge++;
                        continue;
                    }

                    if ((tmp->m2 + minDelay >= cstrDelay) && !((edgeMinDelay.src == i) && (edgeMinDelay.dst == j))) {
                        edgeToRemove[nextEdge].src = i;
                        edgeToRemove[nextEdge].dst = j;
                        nextEdge++;
                        continue;
                    }
                } else {
                    if (tmp->m1 >= cstrIgp) {
                        edgeToRemove[nextEdge].src = i;
                        edgeToRemove[nextEdge].dst = j;
                        nextEdge++;
                        continue;
                    }

                    if (tmp->m2 >= cstrDelay) {
                        edgeToRemove[nextEdge].src = i;
                        edgeToRemove[nextEdge].dst = j;
                        nextEdge++;
                        continue;
                    }
                }
            }
        }
    }

    for (int i = 0 ; i < nextEdge ; i++) {
        graph->succ[edgeToRemove[i].src] = Edge_remove(graph->succ[edgeToRemove[i].src], edgeToRemove[i].dst);
        graph->pred[edgeToRemove[i].dst] = Edge_remove(graph->pred[edgeToRemove[i].dst], edgeToRemove[i].src);
    }
}*/





// long int SrGraph_crash_test(SrGraph_t* graph, Dict_t** dist, my_m1 cstrM1, int nbPlLinks)
// {
//     long int nb_iter = 0;
//     (*dist) = malloc(graph->nbNode * sizeof(Dict_t));
//     ASSERT((*dist), 0);
//     Extendable_t** extendable_path = malloc(graph->nbNode * sizeof(Extendable_t*));
//     ASSERT(extendable_path, 0);
//     my_m1* delays = malloc(graph->nbNode * sizeof(my_m1));
//     memset(delays, 0, graph->nbNode * sizeof(my_m1));
//     my_m2* igp = malloc(graph->nbNode * sizeof(my_m2));
//     memset(igp, 0, graph->nbNode * sizeof(my_m2));

//     for (int j = 0 ; j < graph->nbNode ; j++) {
//         Dict_init(&((*dist)[j]), cstrM1);
//         extendable_path[j] = NULL;
//     }
//     //The only known dist is dist to src
//     Dict_add(&((*dist)[0]), 0, 0);

//     /*src has himself as predecessor*/
//     extendable_path[0] = Extendable_new(0, 0.0, NULL);
//     /*max iter is the min between SEG_MAX and the number of node*/
//     int max_iter = MIN(SEG_MAX, graph->nbNode);

//     for (int i = 1 ; i <= max_iter ; i++) {
//         //printf("ITER : %d -> %ld\n", i, nb_iter);

//         long int real_iter = 0;
//         if (i < 3) {
//             real_iter = MIN(my_pow(graph->nbNode, i-1) * my_pow(nbPlLinks, i), cstrM1);
//         } else {
//             real_iter = cstrM1;
//         }

//         /*for all edges in th graph*/
//         /*#pragma omp parallel
//         {
//             #pragma omp for*/
//             for (int edgeDst = 0 ; edgeDst < graph->nbNode ; edgeDst++) {
//                 /*if (edgeDst == 0) {
//                     continue;
//                 }*/

//                 for (int edgeSrc = 0 ; edgeSrc < graph->nbNode ; edgeSrc++) {
//                     for (Extendable_t* sm_s_eSrc = extendable_path[edgeSrc] ; sm_s_eSrc != NULL ; sm_s_eSrc = sm_s_eSrc->next) {
//                         for (Edge_t* edge = graph->pred[edgeDst][edgeSrc] ; edge != NULL ; edge = edge->next) {
//                             delays[edgeDst] = sm_s_eSrc->infos.m1 + edge->m1;
//                             igp[edgeDst] = sm_s_eSrc->infos.m2 + edge->m2;
//                             //printf("après soucis\n");
//                             /*if the path violate the constraints exit the iteration*/
//                                 /*update the distances*/
//                             Dict_add(&((*dist)[edgeDst]), delays[edgeDst], igp[edgeDst]);
//                         }
//                     }
//                 }
//             }

//             //#pragma omp barrier


//             //#pragma omp for
//             for (int edgeDst = 0 ; edgeDst < graph->nbNode ; edgeDst++) {
//                 Dict_reduce_to_pareto_crash_test(&((*dist)[edgeDst]), i);
//                 Extendable_free(extendable_path[edgeDst]);
//                 extendable_path[edgeDst] = NULL;
//                 for (long int k = 0 ; k < real_iter ; k++) {
//                     extendable_path[edgeDst] = Extendable_new(1, 1, extendable_path[edgeDst]);
//                 }
//             }

//         //}


//         /* copy the new extendable paths to the olds for next iteration*/

//     }
//     for (int i = 0 ; i < graph->nbNode ; i++) {
//         Extendable_free(extendable_path[i]);
//         extendable_path[i] = NULL;
//     }
//     free(extendable_path);
//     free(igp);
//     free(delays);
//     return nb_iter;
// }




/*void retrieve_path(SrGraph* graph, Dict_path** paths, Dict** preds, int src)
{
    (*paths) = malloc(graph->nbNode * sizeof(Dict));

    //for each node
    for (int i = 0 ; i < graph->nbNode ; i++) {
        init_Dict_path(&(*paths)[i]);

        //for each predecessor of the node
        #if Dict_TYPE == Dict_LINKED_LIST
        for (Dict* simple_weight = preds[i] ; simple_weight != NULL ; simple_weight = simple_weight->next) {
            my_m1 weight_to_pred = simple_weight->key.key;

        #endif

        #if Dict_TYPE == Dict_ARRAY_LIST
        if (preds[i] == NULL) {
            continue;
        }

        for (int j = 0 ; j < preds[i]->actual_size ; j++) {
            if (preds[i]->keys[j] == NULL) {
                continue;
            }


            int weight_to_pred = j;
            Dict* simple_weight = preds[i];
        #endif
            int currNode = i;
            Edge* path = NULL;
            //get the predecessor of the node
            int pred = get_value_by_key_node(simple_weight, weight_to_pred);
            int neighType = get_neigh_type_by_key_node(simple_weight, weight_to_pred);
            infos* inf = NULL;
            if (currNode != pred) {
                inf = getInfosEdge(graph->succ, currNode, pred, neighType);

                path = Edge_add(new_infos(currNode, inf->igp_weight, inf->delay, neighType), path);
            }
            //while we don't join source node, retrieve the predecessors
            while (pred != src) {
                weight_to_pred = weight_to_pred - get_infos_SrGraph(graph, currNode, pred, neighType)->igp_weight;
                currNode = pred;
                pred = get_value_by_key_node(preds[currNode], weight_to_pred);
                neighType = get_neigh_type_by_key_node(preds[currNode], weight_to_pred);
                inf = getInfosEdge(graph->succ, currNode, pred, neighType);
                path = Edge_add(new_infos(currNode, inf->igp_weight, inf->delay, inf->seg_type), path);
            }
            path = Edge_add(new_infos(src, 0, 0.0, OSEF_SEGMENT), path);
            path = reverse_Edge(path);
            // add the founded path to the others paths
            #if Dict_TYPE == Dict_LINKED_LIST
            (*paths)[i] = update_Dict_path((*paths)[i], simple_weight->key.key, path);

            #endif

            #if Dict_TYPE == Dict_ARRAY_LIST
            (*paths)[i] = update_Dict_path((*paths)[i], j, path);

            #endif
        }
    }
}*/

SrGraph_t* SrGraph_get_biggest_connexe_coponent(SrGraph_t* sr)
{
    int* nbNeighbors = malloc(sr->nbNode * sizeof(int));
    ASSERT(nbNeighbors, NULL);

    memset(nbNeighbors, 0, sr->nbNode * sizeof(int));

    for (int i = 0 ; i < sr->nbNode ; i++) {
        for (int j = 0 ; j < sr->nbNode ; j++) {
            for (Edge_t* edge = sr->pred[i][j] ; edge != NULL ; edge = edge->next) {
                if (edge->id == 1 && edge->m1 != INF) {
                    nbNeighbors[i]++;
                }
            }
        }
    }

    int maxNeighbor = 0;
    for (int i = 0 ; i < sr->nbNode ; i++) {
        maxNeighbor = MAX(maxNeighbor, nbNeighbors[i]);
    }

    int* nodes = malloc(sr->nbNode * sizeof(int));
    ASSERT(nodes, NULL);

    int nbNodes = 0;

    memset(nodes, 0, sr->nbNode * sizeof(int));

    for (int i = 0 ; i < sr->nbNode ; i++) {
        if (nbNeighbors[i] == maxNeighbor) {
            nodes[nbNodes++] = i;
        }
    }

    SrGraph_t* newSr = SrGraph_init(nbNodes);

    for (int i = 0 ; i < nbNodes ; i++) {
        for (int j = 0 ; j < nbNodes ; j++) {
            //newSr->succ[i][j] = Edge_copy(sr->succ[nodes[i]][nodes[j]]);
            newSr->pred[i][j] = Edge_copy(sr->pred[nodes[i]][nodes[j]]);
        }
    }

    SrGraph_free(sr);
    free(nbNeighbors);
    free(nodes);

    return newSr;
}



bool hasapath(Pfront_t** pfront, int dst)
{
    for (int i = 0 ; i <= SEG_MAX ; i++) {
        if (pfront[i][dst].heapSize != 0) {
            return true;
        }
    }

    return false;
}
