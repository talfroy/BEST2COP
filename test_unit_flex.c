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
    Topology_t* topo = Topology_load_from_file("test_flex.isp", 1, 0);
    SrGraph_t* flex = SrGraph_create_flex_algo(topo);

    SrGraph_print_in_file(flex, stdout);

    SrGraph_free(flex);
    Topology_free(topo);
    return 0;
}