#include "../include/Latrix.h"


void latrix_init(Latrix* l, int nb_nodes){
    l->Latrix = malloc(sizeof(EdgeList[nb_nodes][nb_nodes]));
    for (int i = 0; i<nb_nodes; i++) {
        for (int j = 0; j<nb_nodes; j++) {
            l->Latrix[i][j] = malloc(sizeof(EdgeList));
        }
    }
    l->nb_nodes = nb_nodes;
}




void free_latrix(Latrix* l){
    free(l);
}

void add_to_latrix(Latrix* l, int u, int v, my_m1 m1, my_m2 m2){
    EdgeList* cursor = l->Latrix[u][0];
    EdgeList* prev = l->Latrix[u][0];
    while (cursor->next != NULL && cursor->next->y < v) {
        prev = cursor;
        cursor = cursor->next;
    }
    EdgeList* curr = cursor;
    cursor = cursor->next;
    prev->next = curr;
    curr->next=cursor;
    curr->m1 = m1;
    curr->m2 = m2;
    curr->edge_present = 1;
}

void remove_all_succ(Latrix* l, int u){
    EdgeList* cursor = l->Latrix[u][0];
    while (cursor->next != NULL) {
        cursor->edge_present = 0;
        EdgeList* save = cursor;
        save->next = NULL;
        cursor = cursor -> next;
    }
}
