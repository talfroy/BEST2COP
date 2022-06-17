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

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        ERROR("%s [area 0] [area 1] [leaf_label] [abr_label]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    Topology_t *t1, *t2;
    SrGraph_t *sr1, *sr2;

    t1 = Topology_load_from_file(argv[1], 1, true);
    t2 = Topology_load_from_file(argv[2], 1, true);

    INFO("Topologies succesfully loaded\n");

    sr1 = SrGraph_create_from_topology_best_m2(t1);
    sr2 = SrGraph_create_from_topology_best_m2(t2);

    INFO("Sr graphs succesfully computed\n");

    int abr = LabelTable_get_id(t1->labels, argv[4]);
    int leaf = LabelTable_get_id(t2->labels, argv[3]);

    Dict_t **pf1 = NULL, **pf2 = NULL;
    Pfront_t **pfront1 = NULL, **pfront2 = NULL;

    int iter1 = Best2cop(&pfront1, &pf1, sr1, abr, 1000, M2_INF, 1000, false, NULL);
    int iter2 = Best2cop(&pfront2, &pf2, sr2, leaf, 1000, M2_INF, 1000, false, NULL);

    INFO("End of best2cops\n");

    struct segment_list ***sl1 = Dict_retreive_paths(pf1, sr1, iter1, abr);
    struct segment_list ***sl2 = Dict_retreive_paths(pf2, sr2, iter2, leaf);

    compact_front *cf1 = compact_to_array_2D(pfront1, pf1, iter1, sr1->nbNode, sl1);
    compact_front *cf2 = compact_to_array_2D(pfront2, pf2, iter2, sr2->nbNode, sl2);

    print_compact_array_2D(cf1);
    print_compact_array_2D(cf2);

    abr = LabelTable_get_id(t2->labels, argv[4]);

    Dict_seglist_t **merged = cart(cf2, cf1, 1000, abr);

    for (int j = 0; j <= SEG_MAX; j++)
    {
        for (int k = 0; k < sr1->nbNode; k++)
        {
            Pfront_free(&pfront1[j][k]);
            Dict_free(&pf1[j][k]);
        }

        for (int k = 0; k < sr2->nbNode; k++)
        {
            Pfront_free(&pfront2[j][k]);
            Dict_free(&pf2[j][k]);
        }

        for (int k = 0; k < cf1->nbNodes; k++)
        {
            Dict_seglist_free(&merged[j][k]);
        }
        free(merged[j]);

        free(pfront1[j]);
        free(pf1[j]);
        free(pfront2[j]);
        free(pf2[j]);
    }


    segment_list_free(sl1, iter1, sr1->nbNode);
    free(pfront1);
    free(pf1);
    segment_list_free(sl2, iter2, sr2->nbNode);
    free(pfront2);
    free(pf2);
    free(merged);

    Topology_free(t1);
    Topology_free(t2);
    SrGraph_free(sr1);
    SrGraph_free(sr2);

    return EXIT_SUCCESS;
}