#include "../include/Segment_list.h"

void Dict_seglist_free(Dict_seglist_t* dic)
{
    free(dic->paths);
    free(dic->seg_list);
}


void Dict_seglist_init(Dict_seglist_t* dic, int size)
{
    dic->paths = malloc(size * sizeof(int));
    ASSERT(dic->paths, , size);

    dic->seg_list = malloc(size * sizeof(struct segment_list));
    ASSERT(dic->seg_list, , size);
    
    for (int i = 0 ; i < size ; i++) {
        dic->paths[i] = INF;
    }
    dic->size = size;
}


void Dict_seglist_add(Dict_seglist_t* dic, my_m1 key, my_m2 value, struct segment_list seglist)
{
    if (key < dic->size) {
        dic->paths[key] = value;
        dic->seg_list[key] = seglist;
    }
}

void Dict_reset(Dict_t* d)
{
    for (int i = 0 ; i < d->size ; i++) {
        d->paths[i] = INF;
    }
}



void Segment_list_print(FILE* stream, struct segment_list* sl, Topology_t* topo1, Topology_t* topo2)
{
    if (topo2) {
        fprintf(stream, " [");
        for (int i = 0 ; i < sl->size; i++) {
            if (i <= sl->abr_index) {
                if (i < sl->size - 1) {
                    fprintf(stream, " %s ,", LabelTable_get_name(topo1->labels, sl->seg[i]));
                } else {
                    fprintf(stream, " %s ", LabelTable_get_name(topo1->labels, sl->seg[i]));
                }
            } else {
                if (i < sl->size - 1) {
                    fprintf(stream, " %s ,", LabelTable_get_name(topo2->labels, sl->seg[i]));
                } else {
                    fprintf(stream, " %s ", LabelTable_get_name(topo2->labels, sl->seg[i]));
                }
            }
        }
        fprintf(stream, "]");
    } else {
        fprintf(stream, " [");
        for (int i = 0 ; i < sl->size; i++) {
            if (i < sl->size - 1) {
                fprintf(stream, " %s ,", LabelTable_get_name(topo1->labels, sl->seg[i]));
            } else {
                fprintf(stream, " %s ", LabelTable_get_name(topo1->labels, sl->seg[i]));
            }
        }
        fprintf(stream, "]");
    }
}


void Segment_list_print_id(FILE* stream, struct segment_list *sl)
{
    fprintf(stream, " [");
    for (int i = 0 ; i < sl->size; i++) {
        if (i < sl->size - 1) {
            fprintf(stream, " %d ,", sl->seg[i]);
        } else {
            fprintf(stream, " %d ", sl->seg[i]);
        }
    }
    fprintf(stream, "]");
}




void segment_list_free(struct segment_list*** sl, int maxiter, int nbNodes){
    for (int j = 0 ; j <= maxiter ; j++) {
        for (int i = 0 ; i < nbNodes ; i++) {
            free(sl[j][i]);
        }
        free(sl[j]);
    }
    free(sl);
}

struct segment_list*** Segment_list_retreive_paths(Dict_t** d, SrGraph_t* sr, int maxiter, short src)
{
    struct segment_list*** sl = calloc(maxiter + 1, sizeof(struct segment_list**));
    for (int j = 0 ; j <= maxiter ; j++) {
        sl[j] = calloc(sr->nbNode, sizeof(struct segment_list*));
        for (int i = 0 ; i < sr->nbNode ; i++) {
            sl[j][i] = calloc(1000, sizeof(struct segment_list));
        }
    }
    short pred = 0;
    struct segment_list* stack[10];
    memset(stack, 0, 10 * sizeof(struct segment_list*));
    int  stacksize = -1;
    int curr_iter = -1;
    my_m1 n_d1 = 0;
    my_m2 n_d2 = 0;

    /* for each segment list length */
    for (short iter = maxiter ; iter > 0 ; iter--) {
        /* for each destination node */
        for (short v = 0 ; v < sr->nbNode ; v++) {
            if (v == src) {
                continue;
            }
            /*for each available distance */
            for (my_m1 dist = 0 ; dist < d[iter][v].size ; dist++) {
                if (d[iter][v].paths[dist] == INF) {
                    continue;
                }

                /* if seglist already computed, skip */
                if (sl[iter][v][dist].size) {
                    continue;
                }

                /* init retreive loop */
                pred = v;
                n_d1 = dist;
                n_d2 = d[iter][v].paths[dist];
                curr_iter = iter;
                stacksize = 0;
                memset(stack, 0, 10 * sizeof(struct segment_list*));
                short p_pred = 0;
                my_m2 p_n_d1 = 0;
                my_m2 p_n_d2 = 0;

                while (pred != src) {
                    if (!sl[curr_iter][pred][n_d1].size) {
                        stack[stacksize++] = &sl[curr_iter][pred][n_d1];
                    }
                    p_pred = d[curr_iter][pred].preds[n_d1];
                    curr_iter--;

                    for (Edge_t* l = sr->pred[pred][p_pred] ; l ; l = l->next) {
                        p_n_d1 = n_d1 - l->m1;
                        p_n_d2 = n_d2 - l->m2;

                        if (d[curr_iter][p_pred].paths[p_n_d1] != p_n_d2) {
                            continue;
                        }


                        for (int i = 0 ; i < stacksize ; i++) {
                            stack[i]->seg[stack[i]->size] = pred;
                            stack[i]->seg_type[stack[i]->size] = l->seg_type;
                            stack[i]->size++;
                        }

                        pred = p_pred;
                        n_d1 = p_n_d1;
                        n_d2 = p_n_d2;
                        break;
                    }

                    if (pred != p_pred) {
                        ERROR("Can't find path\n");
                        break;
                    }
                }
            }
        }
    }

    return sl;
}


void print_segment_list(struct segment_list*** sl, int maxiter, int nbNodes)
{
    for (int iter = 0 ; iter <= maxiter ; iter++) {
        for (int v = 0 ; v < nbNodes ; v++) {
            INFO("For Node %d at iter %d:\n", v, iter);
            for (int dist = 0 ; dist < 1000 ; dist++) {
                if (sl[iter][v][dist].size) {
                    printf("Size %d : ", sl[iter][v][dist].size);
                    for (int i = 0 ; i < sl[iter][v][dist].size ; i++) {
                        printf(" %d ", sl[iter][v][dist].seg[i]);
                    }
                    printf("\n");
                }
            }
        }
    }
}