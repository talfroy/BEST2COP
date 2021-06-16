#include "../include/Pfront.h"


int cmp(const void* a, const void* b)
{
    const int* pa = a;
    const int* pb = b;
    return *pa - *pb;
}


void Pfront_init(Pfront_t* bp, int size)
{
    bp->heapSize = 0;
    bp->keys = malloc(size * sizeof(my_m1));
    ASSERT(bp->keys, , size);
    bp->maxSize = size;
}


void Pfront_insert_key(Pfront_t* bp, my_m1 key)
{
    if (bp->heapSize < bp->maxSize) {
        bp->keys[bp->heapSize] = key;
        bp->heapSize++;
    }
}


void Pfront_sort(Pfront_t* bp)
{
    qsort(bp->keys, bp->heapSize, sizeof(my_m1), cmp);
}


int Pfront_union(Pfront_t* pfront, Pfront_t* pfcand)
{
    if (pfront->heapSize + pfcand->heapSize > pfront->maxSize) {
        return pfront->maxSize;
    } else {
        return pfront->heapSize + pfcand->heapSize;
    }
}


void Pfront_print(Pfront_t* pf, FILE* out)
{
    for (int i = 0 ; i < pf->heapSize ; i++) {
        fprintf(out, " %d ", pf->keys[i]);
    }
    printf("\n");
}

Pfront_t* Pfront_merge_sort(Pfront_t* pfront, Pfront_t* pfcand)
{
    Pfront_t* ret = malloc(sizeof(Pfront_t));
    Pfront_init(ret, pfront->maxSize);

    int pfrontIndex = 0;
    int pfcandIndex = 0;

    while (pfrontIndex < pfront->heapSize && pfcandIndex < pfcand->heapSize) {
        if (pfront->keys[pfrontIndex] < pfcand->keys[pfcandIndex]) {
            Pfront_insert_key(ret, pfront->keys[pfrontIndex]);
            pfrontIndex++;
        } else if (pfront->keys[pfrontIndex] > pfcand->keys[pfcandIndex]) {
            Pfront_insert_key(ret, pfcand->keys[pfcandIndex]);
            pfcandIndex++;
        } else {
            Pfront_insert_key(ret, pfcand->keys[pfcandIndex]);
            pfcandIndex++;
            pfrontIndex++;
        }
    }


    while (pfrontIndex < pfront->heapSize) {
        Pfront_insert_key(ret, pfront->keys[pfrontIndex]);
        pfrontIndex++;
    }


    while (pfcandIndex < pfcand->heapSize) {
        Pfront_insert_key(ret, pfcand->keys[pfcandIndex]);
        pfcandIndex++;
    }

    return ret;
}

void Pfront_free(Pfront_t* bh)
{
    free(bh->keys);
}