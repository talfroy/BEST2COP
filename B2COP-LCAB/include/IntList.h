#ifndef __INTLIST_H__
#define __INTLIST_H__


typedef struct IntList_s IntList_t;

struct IntList_s
{
    int value;
    IntList_t *next;
};

void IntListAdd(int value, IntList_t **list);
void IntListFree(IntList_t *list);
void IntListPrint(IntList_t *l);

#endif