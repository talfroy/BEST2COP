#ifndef __EDGE_H__
#define __EDGE_H__

#include "base.h"
#include "params.h"


typedef struct Edge_s Edge_t;

struct Edge_s {
    my_m1 m1;
    my_m2 m2;
    char id;
    Edge_t* next;
};


Edge_t* Edge_new(Edge_t* prev, my_m1 m1, my_m2 m2);

Edge_t* Edge_new_force_id(Edge_t* prev, my_m1 m1, my_m2 m2, char id);

Edge_t* Edge_add(Edge_t* prev, my_m1 m1, my_m2 m2);

Edge_t* Edge_copy(Edge_t* old);

void Edge_free(Edge_t* edge);

#endif