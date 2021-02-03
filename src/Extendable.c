#include "../include/Extendable.h"




Extendable_t* Extendable_new(my_m1 m1, my_m2 m2, Extendable_t* next)
{
    Extendable_t* new = malloc(sizeof(Extendable_t));

    ASSERT(new, NULL);

    new->infos.m1 = m1;
    new->infos.m2 = m2;
    new->next = next;
    return new;
}


void Extendable_free(Extendable_t* ext)
{
    if (ext == NULL) {
        return;
    }

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
        newExt = Extendable_new(tmp->infos.m1, tmp->infos.m2, newExt);
    }

    return newExt;
}


Extendable_list_t* Extendable_list_new(Extendable_list_t* next, int node, Extendable_t* ext)
{
    Extendable_list_t* new_e = malloc(sizeof(Extendable_list_t));
    ASSERT(new_e, NULL);

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