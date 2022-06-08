#include "../include/Pfront.h"


int cmp(const void* a, const void* b)
{
    const int* pa = a;
    const int* pb = b;
    return *pa - *pb;
}


void Pfront_init(Pfront_t* bp, int size, int msd)
{
    bp->heapSize = calloc(msd, sizeof(int));
    
    for (int i = 0 ; i < msd ; i++) {
        bp->heapSize[i] = 0;
    }

    bp->keys = calloc(msd, sizeof(my_m1*));
    for (int i = 0 ; i < msd ; i++) {
        bp->keys[i] =  calloc(size, sizeof(my_m1));
    }

    for (int i = 0 ; i < msd ; i++) {
        for (int j = 0 ; j < size ; j++) {
            bp->keys[i][j] = INF;
        }
    }

    bp->maxSize = size;
}


void Pfront_insert_key(Pfront_t* bp, int key_seg, my_m1 key)
{
    if (bp->heapSize[key_seg] < bp->maxSize) {
        bp->keys[key_seg][bp->heapSize[key_seg]] = key;
        bp->heapSize[key_seg] = bp->heapSize[key_seg] + 1;
    }
}


// void Pfront_sort(Pfront_t* bp)
// {
//     qsort(bp->keys, bp->heapSize, sizeof(my_m1), cmp);
// }


// int Pfront_union(Pfront_t* pfront, Pfront_t* pfcand)
// {
//     if (pfront->heapSize + pfcand->heapSize > pfront->maxSize) {
//         return pfront->maxSize;
//     } else {
//         return pfront->heapSize + pfcand->heapSize;
//     }
// }


void Pfront_print(Pfront_t* pf, FILE* out, int msd)
{
    for (int i = 0 ; i < msd ; i++) {
        for (int j = 0 ; j < pf->heapSize[i] ; j++) {
            fprintf(out, " (%d ; %d) ", i, pf->keys[i][j]);
        }
    }
    fprintf(out, "\n");

}

// Pfront_t* Pfront_merge_sort(Pfront_t* pfront, Pfront_t* pfcand)
// {
//     Pfront_t* ret = malloc(sizeof(Pfront_t));
//     Pfront_init(ret, pfront->maxSize);

//     int pfrontIndex = 0;
//     int pfcandIndex = 0;

//     while (pfrontIndex < pfront->heapSize && pfcandIndex < pfcand->heapSize) {
//         if (pfront->keys[pfrontIndex] < pfcand->keys[pfcandIndex]) {
//             Pfront_insert_key(ret, pfront->keys[pfrontIndex]);
//             pfrontIndex++;
//         } else if (pfront->keys[pfrontIndex] > pfcand->keys[pfcandIndex]) {
//             Pfront_insert_key(ret, pfcand->keys[pfcandIndex]);
//             pfcandIndex++;
//         } else {
//             Pfront_insert_key(ret, pfcand->keys[pfcandIndex]);
//             pfcandIndex++;
//             pfrontIndex++;
//         }
//     }


//     while (pfrontIndex < pfront->heapSize) {
//         Pfront_insert_key(ret, pfront->keys[pfrontIndex]);
//         pfrontIndex++;
//     }


//     while (pfcandIndex < pfcand->heapSize) {
//         Pfront_insert_key(ret, pfcand->keys[pfcandIndex]);
//         pfcandIndex++;
//     }

//     return ret;
// }

void Pfront_free(Pfront_t* bh, int msd)
{
    for (int i = 0 ; i < msd ; i++) {
        free(bh->keys[i]);
    }
}


bool hasapath(Pfront_t** pfront, int dst)
{
    for (int i = 0 ; i <= SEG_MAX ; i++) {
        if (pfront[i][dst].heapSize != 0) {
            return true;
        }
    }

    return false;
}