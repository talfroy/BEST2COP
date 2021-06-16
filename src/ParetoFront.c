#include "../include/ParetoFront.h"



ParetoFront_t* ParetoFront_new(ParetoFront_t* prev, my_m1 m1, my_m2 m2)
{
    ParetoFront_t* new_pf = malloc(sizeof(ParetoFront_t));
    ASSERT(new_pf, NULL, 1);

    new_pf->m1 = m1;
    new_pf->m2 = m2;
    new_pf->next = prev;

    return new_pf;
}


void ParetoFront_free(ParetoFront_t* pf)
{
    if (pf == NULL) {
        return;
    }

    ParetoFront_free(pf->next);
    free(pf);
}