#include "../include/Dict.h"



void Dict_init(Dict_t* dic, int size)
{
    dic->paths = malloc(size * sizeof(int));
    ASSERT_VOID(dic->paths);

    dic->preds = malloc(size * sizeof(short));
    ASSERT_VOID(dic->preds);
    
    for (int i = 0 ; i < size ; i++) {
        dic->paths[i] = INF;
    }
    dic->size = size;
}


void Dict_add(Dict_t* dic, my_m1 key, my_m2 value, short pred)
{
    if (key < dic->size) {
        dic->paths[key] = value;
        dic->preds[key] = pred;
    }
}


void Dict_reduce_to_pareto_crash_test(Dict_t* dic, int nb_seg)
{
    my_m2 last = INF;
    for (my_m1 i = 0 ; i <= nb_seg * dic->size / SEG_MAX ; i++) {
        if (dic->paths[i] != INF) {
            if (dic->paths[i] < last) {
                last = dic->paths[i];
            }
        }
    }
}





void Dict_print(Dict_t* dic)
{
    printf("Dict : ");
    for (my_m1 i = 0 ; i < MAX_SIZE ; i++) {
        if (dic->paths[i] != INF) {
            printf("(%d ; %d) ", i, dic->paths[i]);
        }
    }
    printf("\n\n\n");
}


void init_Dict_path(DictPath* dic)
{
    memset(dic->paths, 0, MAX_SIZE * sizeof(Llist_t*));
}

void add_Dict_path(DictPath* dic, my_m1 key, Llist_t* path)
{
    dic->paths[key] = path;
}


void Dict_free(Dict_t* dic)
{
    free(dic->paths);
    free(dic->preds);
}


