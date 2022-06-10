#include "../include/Extendable.h"




Extendable_t* Extendable_new(my_m0 m0, my_m1 m1, my_m2 m2, int curDag, int lastSegment, Extendable_t* next)
{
    Extendable_t* new = malloc(sizeof(Extendable_t));
    // printf("in Extendable new: %d, %d, %d, %d\n", m0, m1, m2, curDag);
    ASSERT(new, NULL, 1);
    new->infos.m0 = m0;
    new->infos.m1 = m1;
    new->infos.m2 = m2;
    new->infos.lastSegment = lastSegment;
    new->infos.currDag = curDag;
    new->next = next;
    return new;
}


void Extendable_free(Extendable_t* ext)
{
    if (ext == NULL) {
        return;
    }
    if (ext->next != NULL)
        Extendable_free(ext->next);
    free(ext);
}

void Extendable_print(Extendable_t* ext)
{
    if (ext == NULL) {
        return;
    }

    for (Extendable_t* tmp = ext ; tmp != NULL ; tmp = tmp->next) {
        printf(" (%d ; %d) ", tmp->infos.m1, tmp->infos.m2);
    }
        printf("\n");
}


bool Extendable_is_empty(Extendable_t** ext, int nbNode)
{
    for (int i = 0 ; i < nbNode ; i++){
        if (ext[i] != NULL) {
            return false;
        }
    }

    return true;
}


Extendable_t* Extendable_copy(Extendable_t* ext)
{
    Extendable_t* newExt = NULL;

    for (Extendable_t* tmp = ext ; tmp != NULL ; tmp = tmp->next) {
        newExt = Extendable_new(tmp->infos.m0, tmp->infos.m1, tmp->infos.m2, tmp->infos.currDag, tmp->infos.lastSegment, newExt);
    }

    return newExt;
}

void Extendable_add(Extendable_t* og, Extendable_t* to_add){
    Extendable_t* end;
    for (end = og ; end != NULL ; end = og->next) {

    }
    og->next = Extendable_copy(to_add);
    
}


Extendable_list_t* Extendable_list_new(Extendable_list_t* next, int node, Extendable_t* ext)
{
    Extendable_list_t* new_e = calloc(1,sizeof(Extendable_list_t));
    ASSERT(new_e, NULL, 1);

    new_e->node = node;
    new_e->ext = ext;
    new_e->next = next;

    return new_e;
}


void Extendable_list_free(Extendable_list_t* next)
{
    if (next == NULL) {
        return;
    }

    Extendable_list_free(next->next);
    Extendable_free(next->ext);
    free(next);
}