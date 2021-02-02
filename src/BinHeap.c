#include "../include/BinHeap.h"


void BinHeap_init(BinHeap_t* bp, int size)
{
    bp->heapSize = 0;
    bp->keys = malloc((size+1) * sizeof(Nodes_t));
    ASSERT_VOID(bp->keys);
    memset(bp->keys, 0, (size + 1) * sizeof(Nodes_t));
    bp->isPresent = malloc((1+size) * sizeof(int));
    ASSERT_VOID(bp->isPresent);
    for (int i = 0 ; i < size + 1 ; i++) {
        bp->isPresent[i] = -1;
    }
    bp->maxSize = size + 1;
}


int parent(int i) { 
    return (i-1)/2;
} 
  
 
int left(int i) { 
    return (2*i + 1); 
} 
  

int right(int i) {
    return (2*i + 2);
}

void swap(Nodes_t* x, Nodes_t* y)
{ 
    Nodes_t temp = *x; 
    *x = *y; 
    *y = temp; 
}

void swap_int(int* x, int* y)
{ 
    int temp = *x; 
    *x = *y; 
    *y = temp; 
}

void BinHeap_insert_key(BinHeap_t* bp, int node, my_m1 m1, my_m2 m2)
{
    if (bp->heapSize >= bp->maxSize) {
        ERROR("Cannot insert key : no more space");
        return;
    }


    if (bp->isPresent[node] == -2) {
        return;
    }
    
    if (bp->isPresent[node] != -1) {
        BinHeap_decrease_key(bp, node, m1, m2);
        return;
    }


    bp->heapSize++;
    int i = bp->heapSize - 1;
    bp->keys[i].nodeId = node;
    bp->keys[i].m1 = m1;
    bp->keys[i].m2 = m2;
    bp->isPresent[node] = i;
    while (i != 0 && bp->keys[parent(i)].m2 > bp->keys[i].m2)
    {
        //printf("Swap act\n");
        swap(&bp->keys[i], &bp->keys[parent(i)]);
        swap_int(&bp->isPresent[bp->keys[i].nodeId], &bp->isPresent[bp->keys[parent(i)].nodeId]);
        i = parent(i);
    }
}


void BinHeap_min_heapify(BinHeap_t* bp, int i) 
{
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < bp->heapSize && bp->keys[l].m2 < bp->keys[i].m2) {
        smallest = l;
    }
    if (r < bp->heapSize && bp->keys[r].m2 < bp->keys[smallest].m2) {
        smallest = r;
    }
    if (smallest != i) {
        swap(&bp->keys[i], &bp->keys[smallest]);
        swap_int(&bp->isPresent[bp->keys[i].nodeId], &bp->isPresent[bp->keys[smallest].nodeId]);
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
    bp->isPresent[root] = -2;
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
    for (int i = 0 ; i < bp->heapSize ; i++) {
        printf(" (%d ; %d ; %d) ", bp->keys[i].nodeId, bp->keys[i].m1, bp->keys[i].m2);
    }
    printf("\n");
}


void BinHeap_decrease_key(BinHeap_t* bp, int node, my_m1 m1, my_m2 m2)
{
    int index = bp->isPresent[node];
    bp->keys[index].m1 = m1;
    bp->keys[index].m2 = m2;

    while (index != 0 && bp->keys[parent(index)].m2 > bp->keys[index].m2) {
        swap(&bp->keys[parent(index)], &bp->keys[index]);
        swap_int(&bp->isPresent[bp->keys[parent(index)].nodeId], &bp->isPresent[bp->keys[index].nodeId]);
        index = parent(index);
    }
}