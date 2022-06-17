#include "../include/Dict.h"



void Dict_init(Dict_t* dic, size_t size)
{
    dic->paths = calloc(size, sizeof(dic->paths[0]));
    ASSERT(dic->paths, , size);

    dic->preds = calloc(size, sizeof(dic->preds[0]));
    ASSERT(dic->preds, , size);
    
    for (size_t i = 0 ; i < size ; i++) {
        dic->paths[i] = M2_INF;
    }
    dic->size = size;
    dic->nb_add = 0;
}

void Dict_reset(Dict_t *dic) 
{
    // bzero(dic->paths, dic->size * sizeof(int));

    for (size_t i = 0 ; i < dic->size ; i++) {
        dic->paths[i] = INF;
    }
    //bzero(dic->preds[0], dic->max_m1 * dic->max_m0 * sizeof(short));
    dic->nb_add = 0;
}


void Dict_add(Dict_t* dic, my_m1 key, my_m2 value, int pred)
{
    if (key < dic->size) {
        dic->paths[key] = value;
        dic->preds[key] = pred;
    }
}


void Dict_print(Dict_t* dic)
{
    printf("Dict : ");
    for (my_m1 i = 0 ; i < MAX_SIZE ; i++) {
        if (dic->paths[i] != INF) {
            printf("(%"M1_FMT" ; %"M2_FMT") ", i, dic->paths[i]);
        }
    }
    printf("\n\n\n");
}


void Dict_free(Dict_t* dic)
{
    free(dic->paths);
    free(dic->preds);
}
