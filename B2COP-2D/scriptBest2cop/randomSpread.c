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

#define NB_NODE         1000
#define MAX_IGP     100000000


int main()
{

    double courbure = log(10.0) / log(9.0);
    double c_delay;
    INFO("END with delay\n");
    double c_igp = get_c(1, MAX_IGP, courbure);

    int v_igp[100000];
    int v_delay[100000];
    INFO("Start Tab initialization\n");
    fill_tab(c_igp, v_igp, courbure, 1, MAX_IGP);

    int spread = 1000;
    c_delay = get_c(1, spread, courbure);
    fill_tab(c_delay, v_delay, courbure, 1, spread);

    Topology_t* topo = Topology_create_random_quentin(NB_NODE, v_delay, v_igp, 10);

    Topology_print(topo, "Test_topo_flex_rnd.isp");
    Topology_free(topo);
    return 0;
}

