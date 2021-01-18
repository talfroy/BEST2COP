#include "../include/Edge.h"


Edge_t* Edge_new(Edge_t* prev, my_m1 m1, my_m2 m2)
{
    Edge_t* new_e = malloc(sizeof(Edge_t));

    new_e->m1 = m1;
    new_e->m2 = m2;
    new_e->next = prev;

    if (prev == NULL) {
        new_e->id = 1;
    } else {
        new_e->id = prev->id + 1;
    }

    return new_e;
}


Edge_t* Edge_new_force_id(Edge_t* prev, my_m1 m1, my_m2 m2, char id)
{
    Edge_t* new_e = malloc(sizeof(Edge_t));

    new_e->m1 = m1;
    new_e->m2 = m2;
    new_e->next = prev;
    new_e->id = id;

    return new_e;
}


Edge_t* Edge_add(Edge_t* prev, my_m1 m1, my_m2 m2)
{
    if (prev == NULL) {
        return Edge_new(NULL, m1, m2);
    }

    if (prev->m1 == m1 && prev->m2 == m2) {
        return prev;
    }

    if (prev->next == NULL) {
        prev->next = Edge_new_force_id(NULL, m1, m2, prev->id + 1);
        return prev;
    }


    prev->next = Edge_add(prev->next, m1, m2);
    return prev;
}


void Edge_free(Edge_t* edge)
{
    if (edge == NULL) {
        return;
    }

    Edge_free(edge->next);
    free(edge);
}


Edge_t* Edge_copy(Edge_t* old)
{
    if (old == NULL) {
        return NULL;
    }

    Edge_t* last = Edge_copy(old->next);
    return Edge_new(last, old->m1, old->m2);
}


void Edge_print_list(Edge_t* list, FILE* output)
{
    for (Edge_t* tmp = list ; tmp != NULL ; tmp = tmp->next) {
        fprintf(output, " (%d ; %d) ", tmp->m1, tmp->m2);
    }
    printf("\n");
}


Edge_t* Edge_merge_flex(Edge_t* best_m1, Edge_t* best_m2)
{
    Edge_t* flex = NULL;

    flex = Edge_new(NULL, best_m1->m1, best_m1->m2);
    if (flex->m1 > best_m2->m1) {
        if (flex->m2 >= best_m2->m2) {
            Edge_free(flex);
            flex = NULL;
        }
        return Edge_new(flex, best_m2->m1, best_m2->m2);
    } else if (flex->m1 < best_m2->m1) {
        if (flex->m2 > best_m2->m2) {
            return Edge_new(flex, best_m2->m1, best_m2->m2);
        }
    } else {
        if (flex->m2 > best_m2->m2) {
            Edge_free(flex);
            flex = Edge_new(NULL, best_m2->m1, best_m2->m2);
        }
    }
    return flex;
}