#include "../include/Llist.h"



Llist_t* Llist_new(Llist_t* next, my_m1 m1, my_m2 m2, int edgeDst, int segType)
{
    Llist_t* new = malloc(sizeof(Llist_t));

    ASSERT(new, NULL, 1);

    new->infos.edgeDst = edgeDst;
    new->infos.m1 = m1;
    new->infos.m2 = m2;
    new->infos.segType = segType;

    new->next = next;
    return new;
}


static bool LinkInfos_compare(my_m1 m1, my_m2 m2, int edgeDst, LinkInfos* infos2)
{
    if (infos2 == NULL) {
        return false;
    }

    if (m2 != infos2->m2) {
        return false;
    }

    if (m1 != infos2->m1) {
        return false;
    }

    if (edgeDst != infos2->edgeDst) {
        return false;
    }

    return true;
}



Llist_t* Llist_add(Llist_t* list, my_m1 m1, my_m2 m2, int edgeDst, int segType)
{
    if (list == NULL)  {
        return Llist_new(NULL, m1, m2, edgeDst, segType);
    }

    if (LinkInfos_compare(m1, m2, edgeDst, &list->infos)) {
        return list;
    }

    list->next = Llist_add(list->next, m1, m2, edgeDst, segType);
    return list;
}


Llist_t* Llist_remove(Llist_t* list, int edgeDst)
{
    if (list == NULL) {
        return NULL;
    }

    if (list->infos.edgeDst == edgeDst) {
        Llist_t* tmp = list->next;
        free(list);
        return tmp;
    }

    list->next = Llist_remove(list->next, edgeDst);
    return list;
}


int Llist_size(Llist_t* list)
{
    if (list == NULL) {
        return 0;
    }

    return 1 + Llist_size(list->next);
}


void Llist_free(Llist_t* list)
{
    if (list == NULL) {
        return;
    }

    Llist_free(list->next);
    free(list);
}


Llist_t* Llist_search(Llist_t* list, int edgeDst, int adjType)
{
    if (list == NULL) {
        return NULL;
    }

    if (list->infos.edgeDst == edgeDst && list->infos.segType == adjType) {
        return list;
    }

    return Llist_search(list->next, edgeDst, adjType);
}

bool Llist_compare(Llist_t* list1, Llist_t* list2)
{
    if (list1 == NULL) {
        if (list2 == NULL) {
            return true;
        } else {
            return false;
        }
    }

    if (list2 == NULL) {
        if (list1 == NULL) {
            return true;
        } else {
            return false;
        }
    }

    if (LinkInfos_compare(list1->infos.m1, list1->infos.m2, list1->infos.edgeDst, &list2->infos)) {
        return Llist_compare(list1->next, list2->next);
    }

    return false;
}


void Llist_print(Llist_t* list, FILE* file)
{
    if (list == NULL) {
        return;
    }
    
    fprintf(file, " %d ", list->infos.edgeDst);
    Llist_print(list->next, file);
}


void Llist_print_label(Llist_t* list, FILE* file, LabelTable_t* hash)
{
    if (list == NULL) {
        return;
    }

    fprintf(file, " %d (%d, %d) ", list->infos.edgeDst, list->infos.m1, list->infos.m2);
    Llist_print_label(list->next, file, hash);
}


void Llist_print_infos(Llist_t* list, FILE* file)
{
    if (list == NULL) {
        return;
    }

    fprintf(file, " %d (%d, %d) ", list->infos.edgeDst, list->infos.m1, list->infos.m2);
    Llist_print_infos(list->next, file);
}


Llist_t* Llist_get_at_pos(Llist_t* list, int pos)
{
    if (list == NULL) {
        return NULL;
    }

    if (pos == 0) {
        return list;
    }

    return Llist_get_at_pos(list->next, pos-1);
}
