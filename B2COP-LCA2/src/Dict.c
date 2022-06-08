#include "../include/Dict.h"



void Dict_init(Dict_t* dic, int size, int msd)
{

    dic->paths = calloc(msd, sizeof(Path*));
    for (int i = 0 ; i < msd; i++) {
        dic->paths[i] = calloc(size, sizeof(Path));
    }

    for (int i = 0 ; i < msd ; i++) {
        for (int j = 0 ; j < size ; j++) {
            dic->paths[i][j].m2 = INF;
        }
    }

    dic->preds = calloc(msd, sizeof(short*));
     for (int i = 0 ; i < msd; i++) {
        dic->preds[i] = calloc(size, sizeof(short));
    }

    dic->max_m1 = size;
    dic->max_m0 = msd;
    dic->nb_add = 0;
}


void Dict_add(Dict_t* dic, my_m0 key_seg, my_m1 key_cost, my_m2 value, int currDag, int lastSegment, short pred)
{
    if (key_seg < dic->max_m0 && key_cost < dic->max_m1) {
        dic->paths[key_seg][key_cost].m2 = value;
        dic->paths[key_seg][key_cost].m0 = key_seg;
        dic->paths[key_seg][key_cost].m1 = key_cost;
        dic->paths[key_seg][key_cost].currDag = currDag;
        dic->paths[key_seg][key_cost].lastSegment = lastSegment;
        dic->preds[key_seg][key_cost] = pred;
    }
}


void Dict_print(Dict_t* dic)
{
    UNUSED(dic);
    // printf("Dict : ");
    // for (my_m1 i = 0 ; i < MAX_SIZE ; i++) {
    //     if (dic->paths[i] != INF) {
    //         printf("(%d ; %d) ", i, dic->paths[i]);
    //     }
    // }
    // printf("\n\n\n");
}


void Dict_free(Dict_t* dic)
{
    free(dic->paths);
    free(dic->preds);
}
