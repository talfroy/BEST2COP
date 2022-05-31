#include "../include/Dict.h"



void Dict_init(Dict_t* dic, int size, int msd)
{
    dic->paths = malloc(msd * sizeof(int**));
    ASSERT(dic->paths, , msd);

    dic->preds = malloc(msd * sizeof(short*));
    ASSERT(dic->preds, , msd);
    
    
    for (int i = 0 ; i < size ; i++) {
        dic->paths[i] = malloc(size * sizeof(int*));
    }
    

    
    dic->max_m1 = size;
    dic->max_m0 = msd;
    dic->nb_add = 0;
}


void Dict_add(Dict_t* dic, my_m0 key_seg, my_m1 key_cost, my_m2 value, int currDag, int lastSegment, short pred)
{
    if (key_seg < dic->max_m0 && key_cost < dic->max_m1) {
        dic->paths[key_seg][key_cost].m2 = value;
        dic->paths[key_seg][key_cost].m1 = key_cost; // TODO: UNNECESSARY ?
        dic->paths[key_seg][key_cost].m0 = key_seg; // TODO: UNNECESSARY ?
        dic->paths[key_seg][key_cost].currDag = currDag;
        dic->paths[key_seg][key_cost].lastSegment = lastSegment;
        dic->preds[key_seg][key_cost] = pred;
    }
}


void Dict_print(Dict_t* dic)
{
   (void) dic;
    // printf("Dict : ");
    // for (my_m1 i = 0 ; i < MAX_SIZE ; i++) {
    //     if (dic->paths[i][j].m2 != INF) {
    //         printf("(%d ; %d) ", i, dic->paths[i]);
    //     }
    // }
    printf("\n\n\n");
}


void Dict_free(Dict_t* dic)
{
    free(dic->paths);
    free(dic->preds);
}
