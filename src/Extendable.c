#include "../include/Extendable.h"


#define INITIAL_SIZE 10


Extendable_t* Extendable_create(void){
    Extendable_t *ext = malloc(sizeof(Extendable_t));
    ASSERT(ext, NULL);
    ext->count = 0;
    ext->available = INITIAL_SIZE;
    ext->infos = calloc(INITIAL_SIZE, sizeof(Path));
    ASSERT(ext->infos, NULL);
    return ext;
}

void Extendable_append(Extendable_t* ext, my_m1 m1, my_m2 m2)
{
    if(ext->count == ext->available)
    {
        size_t new_size = (ext->available + 10)*1.7;
        Path* new_array = realloc(
            ext->infos, 
            new_size * sizeof(Path)
        );
        if(new_array == NULL) {
            fprintf(stderr, "cannot allocate enough memory\n");
            exit(1);
        }
        ext->infos = new_array;
        ext->available = new_size;
    }
    ext->infos[ext->count].m1 = m1;
    ext->infos[ext->count].m2 = m2;
    ext->count++;
}

void Extendable_clear(Extendable_t* ext){
    ext->count = 0;
}

void Extendable_free(Extendable_t* ext)
{
    if (ext == NULL) {
        return;
    }
    free(ext->infos);
    free(ext);
}

void Extendable_print(Extendable_t* ext)
{
    if (ext == NULL) {
        return;
    }

    foreach_extendable(tmp,ext) {
        printf(" (%d ; %d) ", tmp->m1, tmp->m2);
    }
    printf("\n");
}


bool Extendable_is_empty(Extendable_t** ext, int nbNode)
{
    for (int i = 0 ; i < nbNode ; i++){
        if (ext[i] != NULL) {
            return false;
        }
    }

    return true;
}




Extendable_list_t* Extendable_list_create(void)
{

    Extendable_list_t *l = malloc(sizeof(Extendable_list_t));
    ASSERT(l, NULL);

    l->count = 0;
    l->available = INITIAL_SIZE;

    l->node_ext = calloc(INITIAL_SIZE, sizeof(Extendable_list_element_t));
    if(l->node_ext == NULL)
    {
        free(l);
        return NULL;
    }
    return l;
}

void Extendable_list_append(Extendable_list_t* l, int node, const Extendable_t ext) {
    if(l->count == l->available)
    {
        size_t new_size = (l->available + 10)*1.7;
        Extendable_list_element_t* new_array = realloc(
            l->node_ext, 
            new_size * sizeof(Extendable_list_element_t)
        );
        if(new_array == NULL)
        {
            fprintf(stderr, "cannot allocate enough memory\n");
            exit(1);
        }
        l->node_ext = new_array;
        l->available = new_size;
    }

    l->node_ext[l->count].node = node;
    l->node_ext[l->count].ext = ext;
    l->count++;
}

int Extendable_list_is_empty(const Extendable_list_t* extl) {
    return extl->count == 0;
}

void Extendable_list_clear(Extendable_list_t* extl){
    extl->count = 0;
}



void Extendable_list_free(Extendable_list_t* extl)
{
    if (extl == NULL) {
        return;
    }
    free(extl->node_ext);
    free(extl);
}