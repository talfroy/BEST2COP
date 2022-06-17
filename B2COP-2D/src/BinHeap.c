#include "../include/BinHeap.h"

#define NOT_PRESENT SIZE_MAX
#define REMOVED SIZE_MAX-1

void BinHeap_init(BinHeap_t* bp, size_t size)
{
    bp->heapSize = 0;
    bp->keys = malloc((size+1) * sizeof(bp->keys[0]));
    ASSERT_VOID(bp->keys);
    memset(bp->keys, 0, (size + 1) * sizeof(bp->keys[0]));
    bp->isPresent = malloc((1+size) * sizeof(bp->isPresent[0]));
    ASSERT_VOID(bp->isPresent);
    for (size_t i = 0 ; i < size + 1 ; i++) {
        bp->isPresent[i] = NOT_PRESENT;
    }
    bp->maxSize = size + 1;
}


size_t parent(size_t i) { 
    return (i-1)/2;
} 
  
 
size_t left(size_t i) { 
    return (2*i + 1); 
} 
  

size_t right(size_t i) {
    return (2*i + 2);
}

void swap(Nodes_t* x, Nodes_t* y)
{ 
    Nodes_t temp = *x; 
    *x = *y; 
    *y = temp; 
}

void swap_idx(size_t* x, size_t* y)
{ 
    size_t temp = *x; 
    *x = *y; 
    *y = temp; 
}

void BinHeap_insert_key(BinHeap_t* bp, int node, my_m1 m1, my_m2 m2)
{
    if (bp->heapSize >= bp->maxSize) {
        ERROR("Cannot insert key : no more space\n");
        return;
    }


    if (bp->isPresent[node] == REMOVED) {
        return;
    }
    
    if (bp->isPresent[node] != NOT_PRESENT) {
        BinHeap_decrease_key(bp, node, m1, m2);
        return;
    }


    bp->heapSize++;
    size_t i = bp->heapSize - 1;
    bp->keys[i].nodeId = node;
    bp->keys[i].m1 = m1;
    bp->keys[i].m2 = m2;
    bp->isPresent[node] = i;
    while (i != 0 && bp->keys[parent(i)].m2 > bp->keys[i].m2)
    {
        //printf("Swap act\n");
        swap(&bp->keys[i], &bp->keys[parent(i)]);
        swap_idx(&bp->isPresent[bp->keys[i].nodeId], &bp->isPresent[bp->keys[parent(i)].nodeId]);
        i = parent(i);
    }
}


void BinHeap_min_heapify(BinHeap_t* bp, size_t i) 
{
    size_t l = left(i);
    size_t r = right(i);
    size_t smallest = i;
    if (l < bp->heapSize && bp->keys[l].m2 < bp->keys[i].m2) {
        smallest = l;
    }
    if (r < bp->heapSize && bp->keys[r].m2 < bp->keys[smallest].m2) {
        smallest = r;
    }
    if (smallest != i) {
        swap(&bp->keys[i], &bp->keys[smallest]);
        swap_idx(&bp->isPresent[bp->keys[i].nodeId], &bp->isPresent[bp->keys[smallest].nodeId]);
        BinHeap_min_heapify(bp, smallest);
    }
}


int BinHeap_extract_min(BinHeap_t* bp)
{
    
    if (bp->heapSize <= 0) 
        return -1; 
    if (bp->heapSize == 1) 
    { 
        bp->heapSize--; 
        return bp->keys[0].nodeId; 
    }

    int root = bp->keys[0].nodeId;
    bp->keys[0] = bp->keys[bp->heapSize-1];
    bp->isPresent[bp->keys[0].nodeId] = 0;
    bp->isPresent[root] = REMOVED;
    bp->heapSize--;
    BinHeap_min_heapify(bp, 0);

    return root;
}


void BinHeap_free(BinHeap_t* bp)
{
	free(bp->isPresent);
    free(bp->keys);
}


void BinHeap_print(BinHeap_t* bp)
{
    for (size_t i = 0 ; i < bp->heapSize ; i++) {
        printf(" (%d ; %"M1_FMT" ; %"M2_FMT") ", bp->keys[i].nodeId, bp->keys[i].m1, bp->keys[i].m2);
    }
    printf("\n");
}


void BinHeap_decrease_key(BinHeap_t* bp, int node, my_m1 m1, my_m2 m2)
{
    size_t index = bp->isPresent[node];
    bp->keys[index].m1 = m1;
    bp->keys[index].m2 = m2;

    while (index != 0 && bp->keys[parent(index)].m2 > bp->keys[index].m2) {
        swap(&bp->keys[parent(index)], &bp->keys[index]);
        swap_idx(&bp->isPresent[bp->keys[parent(index)].nodeId], &bp->isPresent[bp->keys[index].nodeId]);
        index = parent(index);
    }
}