#include "IntList.h"
#include "base.h"

void IntListAdd(int value, IntList_t **list)
{
    if (!*list)
    {
        IntList_t *new = malloc(sizeof(IntList_t));
        new->next = NULL;
        new->value = value;
        *list = new;
        return;
    }

    IntList_t *cursor = *list;
    if (cursor->value == value)
        return;
    while (cursor->next != NULL)
    {
        if (cursor->value == value)
            return;

        cursor = cursor->next;
    }

    IntList_t *new = malloc(sizeof(IntList_t));
    new->next = NULL;
    new->value = value;
    cursor->next = new;
    return;
}

void IntListPrint(IntList_t *l)
{
    IntList_t *cursor = l;
    while (cursor != NULL)
    {
        printf("%d-", cursor->value);
        cursor = cursor->next;
    }
    printf("\n");
}

void IntListFree(IntList_t *list)
{
    IntList_t *tmp;
    while (list != NULL)
    {
        tmp = list->next;
        free(list);
        list = tmp;
    }
}