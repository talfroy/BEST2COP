#include "../include/Topology.h"


Topology_t* Topology_init(int nbNode)
{
    Topology_t* topo = malloc(sizeof(Topology_t));
    ASSERT(topo, NULL);

    topo->pred = malloc(nbNode * sizeof(Llist_t*));
    ASSERT(topo->pred, NULL);

    topo->succ = malloc(nbNode * sizeof(Llist_t*));
    ASSERT(topo->succ, NULL);

    for (int i = 0 ; i < nbNode ; i++) {
        topo->pred[i] = NULL;
        topo->succ[i] = NULL;
    }

    topo->nbNode = nbNode;

    return topo;
}


Topology_t* Topology_load_from_file(const char* filename, int precision, char biDir)
{
    LabelTable_t labels;
    LabelTable_init(&labels);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        ERROR("Can't find file %s in the current directory\n", filename);
        return NULL;
    }

    int src, dst;
    my_m2 m2;
    char destLabel[128], srcLabel[128];
    char line[1024];
    double m1;
    int nbNode = 0;
    int nbLine = 1;

    while (fgets(line, 1024,  file))
    {
        if (sscanf(line, "%s %s %lf %d\n", &srcLabel[0], &destLabel[0], &m1, &m2) == 4) {
            src = LabelTable_add_node(&labels, srcLabel);
            dst = LabelTable_add_node(&labels, destLabel);
            nbNode = MAX(nbNode, labels.nextNodeId);
        } else if (sscanf(line, "%s %s %d\n", &srcLabel[0], &destLabel[0], &m2) == 3) {
            src = LabelTable_add_node(&labels, srcLabel);
            dst = LabelTable_add_node(&labels, destLabel);
            nbNode = MAX(nbNode, labels.nextNodeId);
        } else {
            ERROR("Can't load line %d : your file might not have the good format : \n\t[source_node]  [destination_node]  [delay]  [IGP_weight]\n", nbLine);
            return NULL;
        }
        nbLine++;
    }
    LabelTable_sort(&labels);

    Topology_t* topo = Topology_init(nbNode);

    if (topo == NULL) {
        ERROR("Topology can't be initialized\n");
        return NULL;
    }

    fclose(file);
    file = fopen(filename, "r");
    
    if (file == NULL) {
        ERROR("Can't find file %s in the current directory\n", filename);
        return NULL;
    }
    nbLine = 1;

    while (fgets(line, 1024,  file))
    {
         if (sscanf(line, "%s %s %lf %d\n", &srcLabel[0], &destLabel[0], &m1, &m2) == 4) {
            src = LabelTable_get_id(&labels, srcLabel);
            dst = LabelTable_get_id(&labels, destLabel);
            m1 *= my_pow(10, precision);
            topo->succ[src] = Llist_new(topo->succ[src], m1, m2, dst, ADJACENCY_SEGMENT);
            topo->pred[dst] = Llist_new(topo->pred[dst], m1, m2, src, ADJACENCY_SEGMENT);
            if (biDir) {
                topo->succ[dst] = Llist_new(topo->succ[dst], m1, m2, src, ADJACENCY_SEGMENT);
                topo->pred[src] = Llist_new(topo->pred[src], m1, m2, dst, ADJACENCY_SEGMENT);
            }
        } else if (sscanf(line, "%s %s %d\n", &srcLabel[0], &destLabel[0], &m2) == 3) {
            src = LabelTable_get_id(&labels, srcLabel);
            dst = LabelTable_get_id(&labels, destLabel);
            topo->succ[src] = Llist_new(topo->succ[src], 0, m2, dst, ADJACENCY_SEGMENT);
            topo->pred[dst] = Llist_new(topo->pred[dst], 0, m2, src, ADJACENCY_SEGMENT);
            if (biDir) {
                topo->succ[dst] = Llist_new(topo->succ[dst], 0, m2, src, ADJACENCY_SEGMENT);
                topo->pred[src] = Llist_new(topo->pred[src], 0, m2, dst, ADJACENCY_SEGMENT);
            }
        } else {
            ERROR("Can't load line %d : your file might not have the good format : \n\t[source_node]  [destination_node]  [delay]  [IGP_weight]\n", nbLine);
            return NULL;
        }
        nbLine++;
    }

    fclose(file);
    LabelTable_free(&labels);
    return topo;
}


long int my_pow(long int x, int y)
{
    if (y == 0) {
        return 1;
    }

    return x * my_pow(x, y-1);
}


LinkInfos* Topology_get_edge_infos(Topology_t* topo, int src, int dst, int adjType)
{
    return &Llist_search(topo->succ[src], dst, adjType)->infos;
}



void Topology_print(Topology_t* topo, char* topoF)
{
    if (topo == NULL) {
        return;
    }
    FILE* file = fopen(topoF, "w");
    printf("PRINT TOPO : succ\n\n");
    for (int i = 0 ; i < topo->nbNode ; i++) {
        for (Llist_t* tmp = topo->succ[i] ; tmp != NULL ; tmp = tmp->next) {
            //printf("Petit test %d\n", tmp->infos.edgeDst);
            fprintf(file, "%d %d %d %d\n", i, tmp->infos.edgeDst, tmp->infos.m1, tmp->infos.m2);
        }
    }

    fclose(file);
    printf("fin d'écriture de la topo\n");
}


void Topology_free(Topology_t* topo)
{
    if (topo == NULL) {
        return;
    }

    for (int i = 0 ; i < topo->nbNode ; i++) {
        Llist_free(topo->succ[i]);
        Llist_free(topo->pred[i]);
    }

    free(topo->succ);
    free(topo->pred);

    free(topo);
}


void dikjstra_best_m2(Edge_t**** succOutGraph, Edge_t**** predOutGraph, Llist_t** ingraph, 
                int root, my_m1** m1dists, my_m2** m2dists, int nbNodes)
{
    BinHeap_t bp;
    BinHeap_init(&bp, nbNodes);

    // if (stack == NULL) {
    //     ERROR("Stack for dijkstra can't be created\n");
    //     return;
    // }

    for (int i = 0 ; i < nbNodes ; i++) {
        (*m1dists)[i] = INF;
        (*m2dists)[i] = INF;
    }
    my_m1 pathM1 = 0;
    my_m2 pathM2 = 0;

    (*m1dists)[root] = 0;
    (*m2dists)[root] = 0;

    int** parents = malloc(nbNodes * sizeof(int*));
    for (int i = 0 ; i < nbNodes ; i++) {
        parents[i] = malloc(NB_NODE_MAX * sizeof(int));
        memset(parents[i], 0, NB_NODE_MAX * sizeof(int));
    }


    for (int currNode = root, nbSeen = 0 ; nbSeen < nbNodes * nbNodes && currNode != -1 ; nbSeen++) {
        for (Llist_t* neighbor = ingraph[currNode] ; neighbor != NULL ; neighbor = neighbor->next) {
            if (neighbor->infos.edgeDst == root || neighbor->infos.m1 == INF || neighbor->infos.m2 == INF) {
                continue;
            }
            if ((neighbor->infos.m1 + (*m1dists)[currNode]) > 10000 || (neighbor->infos.m1 + (*m1dists)[currNode]) < 0) {
                printf("node %d -> %d + %d\n", currNode, neighbor->infos.m1, (*m1dists)[currNode]);
            }
            if ((pathM2 = neighbor->infos.m2 + (*m2dists)[currNode]) == (*m2dists)[neighbor->infos.edgeDst]) {
                if ((pathM1 = neighbor->infos.m1 + (*m1dists)[currNode]) == (*m1dists)[neighbor->infos.edgeDst]) {
                    parents[neighbor->infos.edgeDst][++parents[neighbor->infos.edgeDst][0]] = currNode;
                } else if (pathM1 > (*m1dists)[neighbor->infos.edgeDst]) {
                    (*m1dists)[neighbor->infos.edgeDst] = pathM1;
                    parents[neighbor->infos.edgeDst][0] = 1;
                    parents[neighbor->infos.edgeDst][1] = currNode;
                    //empile(neighbor->infos.edgeDst, stack);
                    BinHeap_insert_key(&bp, neighbor->infos.edgeDst, neighbor->infos.m1, neighbor->infos.m2);
                }
            } else if (pathM2 < (*m2dists)[neighbor->infos.edgeDst]) {
                (*m2dists)[neighbor->infos.edgeDst] = pathM2;
                (*m1dists)[neighbor->infos.edgeDst] = neighbor->infos.m1 + (*m1dists)[currNode];
                parents[neighbor->infos.edgeDst][0] = 1;
                parents[neighbor->infos.edgeDst][1] = currNode;
                //empile(neighbor->infos.edgeDst, stack);
                BinHeap_insert_key(&bp, neighbor->infos.edgeDst, neighbor->infos.m1, neighbor->infos.m2);
            }
        }
        //currNode = depile(stack);
        currNode = BinHeap_extract_min(&bp);
    }
    
    //freeStack(stack);
    BinHeap_free(&bp);

    for (int i = 0 ; i < nbNodes ; i++) {
        if (i != root) {
            (*succOutGraph)[root][i] = Edge_add((*succOutGraph)[root][i], (*m1dists)[i], (*m2dists)[i]);
            (*predOutGraph)[i][root] = Edge_add((*predOutGraph)[i][root], (*m1dists)[i], (*m2dists)[i]);
        }
    }

    for (int i = 0 ; i < nbNodes ; i++) {
        free(parents[i]);
        if ((*m1dists)[i] > 10000 || (*m1dists)[i] < 0) {
            printf("Un soucis de %d -> %d\n", root,i);
        }
    }

    free(parents);
}




void dikjstra_best_m1(Edge_t**** succOutGraph, Edge_t**** predOutGraph, Llist_t** ingraph, 
                int root, my_m1** m1dists, my_m2** m2dists, int nbNodes)
{
    // myStack* stack = initStack(nbNodes);

    // if (stack == NULL) {
    //     ERROR("Stack for dijkstra can't be created\n");
    //     return;
    // }

    BinHeap_t bp;
    BinHeap_init(&bp, nbNodes);

    for (int i = 0 ; i < nbNodes ; i++) {
        (*m1dists)[i] = INF;
        (*m2dists)[i] = INF;
    }
    my_m1 pathM1 = 0;
    my_m2 pathM2 = 0;

    (*m1dists)[root] = 0;
    (*m2dists)[root] = 0;

    int parents[nbNodes][NB_NODE_MAX];

    memset(parents, 0, nbNodes * NB_NODE_MAX * sizeof(int));

    for (int currNode = root, nbSeen = 0 ; nbSeen < nbNodes * nbNodes && currNode != -1 ; nbSeen++) {
        for (Llist_t* neighbor = ingraph[currNode] ; neighbor != NULL ; neighbor = neighbor->next) {
            if (neighbor->infos.edgeDst == root || neighbor->infos.m1 == INF || neighbor->infos.m2 == INF) {
                continue;
            }

            if ((pathM1 = neighbor->infos.m1 + (*m1dists)[currNode]) == (*m1dists)[neighbor->infos.edgeDst]) {
                if ((pathM2 = neighbor->infos.m2 + (*m2dists)[currNode]) == (*m2dists)[neighbor->infos.edgeDst]) {
                    parents[neighbor->infos.edgeDst][++parents[neighbor->infos.edgeDst][0]] = currNode;
                } else if (pathM2 > (*m2dists)[neighbor->infos.edgeDst]) {
                    (*m2dists)[neighbor->infos.edgeDst] = pathM2;
                    parents[neighbor->infos.edgeDst][0] = 1;
                    parents[neighbor->infos.edgeDst][1] = currNode;
                    //empile(neighbor->infos.edgeDst, stack);
                    BinHeap_insert_key(&bp, neighbor->infos.edgeDst, neighbor->infos.m2, neighbor->infos.m1);
                }
            } else if (pathM1 < (*m1dists)[neighbor->infos.edgeDst]) {
                (*m1dists)[neighbor->infos.edgeDst] = pathM1;
                (*m2dists)[neighbor->infos.edgeDst] = neighbor->infos.m2 + (*m2dists)[currNode];
                parents[neighbor->infos.edgeDst][0] = 1;
                parents[neighbor->infos.edgeDst][1] = currNode;
                //empile(neighbor->infos.edgeDst, stack);
                BinHeap_insert_key(&bp, neighbor->infos.edgeDst, neighbor->infos.m2, neighbor->infos.m1);
            }
        }
        currNode = BinHeap_extract_min(&bp);
    }
    
    //freeStack(stack);

    for (int i = 0 ; i < nbNodes ; i++) {
        if (i != root) {
            (*succOutGraph)[root][i] = Edge_add((*succOutGraph)[root][i], (*m1dists)[i], (*m2dists)[i]);
            (*predOutGraph)[i][root] = Edge_add((*predOutGraph)[i][root], (*m1dists)[i], (*m2dists)[i]);
        }
    }
}


int rand_a_b(int a, int b)
{
    return rand()%(b - a) + a;
}


Topology_t* Topology_create_random(int size, int v_delay[], int v_igp[])
{
    Topology_t* topo = Topology_init(size);

    for (int i = 0 ; i < size ; i++) {
        for (int j = i + 1 ; j < size ; j++) {
            if (i == j) {
                continue;
            }
            my_m1 m1 = v_delay[RAND(0, 10000)];
            my_m2 m2 = v_igp[RAND(0, 10000)];
            //printf("add a new arc (%d -> %d)\n", i, j);
            topo->succ[i] = Llist_new(topo->succ[i], m1, m2, j, ADJACENCY_SEGMENT);
            topo->pred[j] = Llist_new(topo->pred[j], m1, m2, i, ADJACENCY_SEGMENT);
            topo->succ[j] = Llist_new(topo->succ[j], m1, m2, i, ADJACENCY_SEGMENT);
            topo->pred[i] = Llist_new(topo->pred[i], m1, m2, j, ADJACENCY_SEGMENT);
        }

    }

    return topo;
}
