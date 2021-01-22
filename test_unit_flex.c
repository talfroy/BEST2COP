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

int main() {
    Topology_t* topo = Topology_load_from_file("geoSprint", 1, 0);
    int* m1dists, *m2dists;
    m1dists = malloc(sizeof(int) * topo->nbNode);
    m2dists = malloc(sizeof(int) * topo->nbNode);
    dikjstra_best_m1(NULL, NULL, topo->succ, 0, &m1dists, &m2dists, topo->nbNode);
    free(m1dists);
    free(m2dists);
    return 0;
}