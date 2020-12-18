#include "../include/BinHeap.h"


int cmp(const void* a, const void* b)
{
    const int* pa = a;
    const int* pb = b;
    return *pa - *pb;
}


void BinHeap_init(BinHeap_t* bp, int size)
{
    bp->heapSize = 0;
    bp->keys = malloc(size * sizeof(my_m1));
    ASSERT_VOID(bp->keys);
    bp->maxSize = size;
}


void BinHeap_insert_key(BinHeap_t* bp, my_m1 key)
{
    if (bp->heapSize < bp->maxSize) {
        bp->keys[bp->heapSize] = key;
        bp->heapSize++;
    }
}


void BinHeap_sort(BinHeap_t* bp)
{
    qsort(bp->keys, bp->heapSize, sizeof(my_m1), cmp);
}


int BinHeap_union(BinHeap_t* pfront, BinHeap_t* pfcand)
{
    if (pfront->heapSize + pfcand->heapSize > pfront->maxSize) {
        return pfront->maxSize;
    } else {
        return pfront->heapSize + pfcand->heapSize;
    }
}

BinHeap_t* BinHeap_merge_sort(BinHeap_t* pfront, BinHeap_t* pfcand)
{
    BinHeap_t* ret = malloc(sizeof(BinHeap_t));
    BinHeap_init(ret, pfront->maxSize);

    int pfrontIndex = 0;
    int pfcandIndex = 0;

    while (pfrontIndex < pfront->heapSize && pfcandIndex < pfcand->heapSize) {
        if (pfront->keys[pfrontIndex] < pfcand->keys[pfcandIndex]) {
            BinHeap_insert_key(ret, pfront->keys[pfrontIndex]);
            pfrontIndex++;
        } else if (pfront->keys[pfrontIndex] > pfcand->keys[pfcandIndex]) {
            BinHeap_insert_key(ret, pfcand->keys[pfcandIndex]);
            pfcandIndex++;
        } else {
            BinHeap_insert_key(ret, pfcand->keys[pfcandIndex]);
            pfcandIndex++;
            pfrontIndex++;
        }
    }


    while (pfrontIndex < pfront->heapSize) {
        BinHeap_insert_key(ret, pfront->keys[pfrontIndex]);
        pfrontIndex++;
    }


    while (pfcandIndex < pfcand->heapSize) {
        BinHeap_insert_key(ret, pfcand->keys[pfcandIndex]);
        pfcandIndex++;
    }

    return ret;
}

void BinHeap_free(BinHeap_t* bh)
{
    free(bh->keys);
}
