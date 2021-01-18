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
#include "include/BinHeap.h"



int main() {
    BinHeap_t bp;
    BinHeap_init(&bp, 50);
    BinHeap_print(&bp);
    BinHeap_insert_key(&bp, 1, 1, 1);
    BinHeap_print(&bp);
    BinHeap_insert_key(&bp, 2, 2, 2);
    BinHeap_insert_key(&bp, 3, 3, 3);
    BinHeap_insert_key(&bp, 5, 5, 5);
    BinHeap_insert_key(&bp, 6, 6, 6);
    BinHeap_insert_key(&bp, 7, 7, 7);
    BinHeap_insert_key(&bp, 8, 8, 8);
    BinHeap_insert_key(&bp, 9, 9, 9);
    BinHeap_insert_key(&bp, 5, 5, 5);
    BinHeap_print(&bp);
    for (int i = 0 ; i < bp.maxSize ; i++) {
        if (bp.isPresent[i] != -1) {
            printf("%d -> %d\n", i, bp.isPresent[i]);
        }
    }
    BinHeap_extract_min(&bp);
    BinHeap_print(&bp);
    for (int i = 0 ; i < bp.maxSize ; i++) {
        if (bp.isPresent[i] != -1) {
            printf("%d -> %d\n", i, bp.isPresent[i]);
        }
    }

    BinHeap_insert_key(&bp, 9, 4, 4);
    BinHeap_print(&bp);
    for (int i = 0 ; i < bp.maxSize ; i++) {
        if (bp.isPresent[i] != -1) {
            printf("%d -> %d\n", i, bp.isPresent[i]);
        }
    }
    return 0;
}