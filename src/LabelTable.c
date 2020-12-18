#include "../include/LabelTable.h"


void LabelTable_init(LabelTable_t* hash)
{
    hash->node = malloc(NB_NODE_ALLOC * sizeof(node));
    ASSERT_VOID(hash->node);

    for (int i = 0 ; i < NB_NODE_ALLOC ; i++) {
        memset(hash->node[i].name, 0, 128);
    }
    
    hash->nextNodeId = 0;
}


int LabelTable_add_node(LabelTable_t* hash, char* name)
{
    for (int i = 0 ; i < hash->nextNodeId ; i++) {
        if (strcmp(name, hash->node[i].name) == 0) {
            return i;
        }
    }

    if ((hash->nextNodeId % NB_NODE_ALLOC) == 0 && hash->nextNodeId != 0) {
        hash->node = realloc(hash->node, (hash->nextNodeId + NB_NODE_ALLOC) * sizeof(node));
        ASSERT(hash->node, -1);

        for (int i = 0 ; i < NB_NODE_ALLOC ; i++) {
            memset(hash->node[hash->nextNodeId + i].name, 0, 128);
        }
    }

    strcpy(hash->node[hash->nextNodeId].name, name);
    hash->nextNodeId++;
    return hash->nextNodeId - 1;
}


int LabelTable_get_id(LabelTable_t* hash, char* name)
{
    for (int i = 0 ; i < hash->nextNodeId ; i++) {
        if (strcmp(name, hash->node[i].name) == 0) {
            return i;
        }
    }

    return -1;
}


char* LabelTable_get_name(LabelTable_t* hash, int nodeId)
{
    if (nodeId > hash->nextNodeId) {
        return NULL;
    }

    return hash->node[nodeId].name;
}


void LabelTable_free(LabelTable_t* hash)
{
    free(hash->node);
    hash->nextNodeId = 0;
}


void LabelTable_print(LabelTable_t* hash)
{
    for (int i = 0 ; i < hash->nextNodeId ; i++) {
        printf("ID %d : %s\n", i , hash->node[i].name);
    }
}


static int compare_hash (void const *a, void const *b)
{
    const node *pa = a;
    const node *pb = b;
    return strcmp (pa->name, pb->name);
}

void LabelTable_sort(LabelTable_t* hash)
{
    qsort(hash->node, hash->nextNodeId, sizeof(node), compare_hash);
}