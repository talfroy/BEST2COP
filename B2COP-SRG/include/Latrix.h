#include "../include/base.h"

struct EdgeList {
    char edge_present;
    my_m1 m1;
    my_m2 m2;
    int x;
    int y;
    struct EdgeList* next;
};
typedef struct EdgeList EdgeList;




struct Latrix {
    EdgeList*** Latrix;
    int nb_nodes;
};
typedef struct Latrix Latrix;