#ifndef __PARETO_FRONT_H__
#define __PARETO_FRONT_H__

#include "base.h"
#include "params.h"


typedef struct ParetoFront_s ParetoFront_t;

struct ParetoFront_s {
    my_m1 m1;
    my_m2 m2;
    ParetoFront_t* next;
};

ParetoFront_t* ParetoFront_new(ParetoFront_t* prev, my_m1 m1, my_m2 m2);

void ParetoFront_free(ParetoFront_t* pf);

#endif