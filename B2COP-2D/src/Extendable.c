#include "../include/Extendable.h"



Extendable_t* Extendable_create(void) 
{
    Extendable_t* new = calloc(1, sizeof(*new));

    new->allocated = 2;
    new->size = 0;
    new->infos = malloc(new->allocated * sizeof(*new->infos));

    return new;
}

void Extendable_add(Extendable_t* next, my_m1 m1, my_m2 m2)
{
    if (next->size == next->allocated) {
        next->allocated = (size_t)(next->allocated * 1.5 + 2);
        next->infos = realloc(next->infos, next->allocated * sizeof(*next->infos));
    }

    next->infos[next->size].m1 = m1;
    next->infos[next->size].m2 = m2;
    next->size++;
}

void Extendable_clear(Extendable_t* next)
{
    next->size = 0;
}

void Extendable_free(Extendable_t* ext)
{
    if(ext->infos) 
        free(ext->infos);

    free(ext);
}

void Extendable_print(Extendable_t* ext)
{
    if (ext == NULL) {
        return;
    }
    for(size_t i = 0 ; i < ext->size ; i++) {
        printf(" (%"M1_FMT" ; %"M2_FMT") ", CAST_M1(ext->infos[i].m1), CAST_M2(ext->infos[i].m2));
    }
    printf("\n");
}


bool Extendable_is_empty(Extendable_t** ext, int nbNode)
{
    for (int i = 0 ; i < nbNode ; i++){
        if (ext[i]->size > 0) {
            return false;
        }
    }

    return true;
}


Extendable_list_t* Extendable_list_create()
{
    Extendable_list_t* new = calloc(1, sizeof(*new));

    new->allocated = 2;
    new->size = 0;
    new->ext = malloc(new->allocated * sizeof(*new->ext));
    new->sources = malloc(new->allocated * sizeof(*new->sources));

    return new;
}
void Extendable_list_add(Extendable_list_t* next, int node, Extendable_t* ext)
{
    if(next->size == next->allocated) {
        next->allocated = (size_t)(next->allocated*1.5 + 2);
        next->ext = realloc(next->ext, next->allocated * sizeof(*next->ext));
        next->sources = realloc(next->sources, next->allocated * sizeof(*next->sources));
    }
    next->ext[next->size] = ext;
    next->sources[next->size] = node;
    next->size++;
}


void Extendable_list_clear(Extendable_list_t* next)
{
    //for (size_t i = 0 ; i < next->size ; i++) {
    //    Extendable_free(next->ext[i]);
    //}
    next->size = 0;
}

size_t Extendable_list_size(Extendable_list_t* next)
{
    return next->size;
}

void Extendable_list_free(Extendable_list_t* next)
{
    if (next == NULL) {
        return;
    }
    //for (size_t i = 0 ; i < next->size ; i++) {
    //    Extendable_free(next->ext[i]);
    //}
    free(next->ext);
    free(next->sources);
    free(next);
}