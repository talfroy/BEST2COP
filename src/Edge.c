#include "../include/Edge.h"


void Edge_new(Edge_tab_t* prev, my_m1 m1, my_m2 m2)
{
    if (!prev->available - prev->size) {
        prev->list = realloc(prev->list, prev->available * 2 * sizeof(Edge_t));
        prev->available *= 2;
    }

    prev->list[prev->size].m1 = m1;
    prev->list[prev->size].m2 = m2;
    prev->size++;
    prev->list[prev->size].id = prev->size;
}


void Edge_tab_init(Edge_tab_t* tab)
{
    tab->list = calloc(1, sizeof(Edge_t));
    tab->size = 0;
    tab->available = 1;
}



void Edge_add(Edge_tab_t* prev, my_m1 m1, my_m2 m2)
{
    Edge_new(prev, m1, m2);
}

void Edge_tab_delete(Edge_tab_t* tab)
{
    free(tab->list);
}

void Edge_print_list(Edge_tab_t* list, FILE* output)
{
    Edge_t edge;
    for (int i = 0 ; i < list->size && (edge = list->list[i], 1) ; i++) {
        fprintf(output, " %d %d ", edge.m1, edge.m2);
    }
    fprintf(output, "\n");
}


void Edge_merge_flex(Edge_tab_t* tab, Edge_t* best_m2)
{
    Edge_t* flex = NULL;

    Edge_new(tab, best_m2->m1, best_m2->m2);
    if (flex->m1 > best_m2->m1) {
        if (flex->m2 >= best_m2->m2) {
            //Edge_free(flex);
            flex = NULL;
        }
        return;
    } else if (flex->m1 < best_m2->m1) {
        if (flex->m2 > best_m2->m2) {
            return ;
        }
    } else {
        if (flex->m2 > best_m2->m2) {
            //Edge_free(flex);
            Edge_new(tab, best_m2->m1, best_m2->m2);
        }
    }
    return;
}