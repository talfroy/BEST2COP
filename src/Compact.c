#include "Compact.h"


// compact_front* compact_front_new(int d, compact_front* next){
// 	compact_front* new = malloc(sizeof(compact_front));
// 	ASSERT(new, NULL);
// 	new -> delay = d;
// 	new -> paths = compact_paths_new();
// 	new -> next = next;
// 	return new;
// }

// compact_paths* compact_paths_new(){
// 	compact_paths* new = malloc(sizeof(compact_paths));
// 	return new;
// }



int* get_nb_paths_per_dest(Dict_t** dist, int nbNodes, int iter)
{
	int* nb_paths = calloc(nbNodes, sizeof(int));
    for (int i = 0 ; i < nbNodes ; i++) {
        for (int k = 0 ; k < iter ; k++) {
            for (int j = 0 ; j < dist[k][i].size ; j++) {
                if (dist[k][i].paths[j] != INF) {
                    nb_paths[i] += 1;
                }
            }
        }
    }
	return nb_paths;
}

void print_compact_array(path** compact_pf){
	int i = 0; 
	while (compact_pf[i] != NULL) {
		int j = 0;
		while (compact_pf[i][j].cost != -1){
			printf("Nodes %d : %d %d %d\n", i, compact_pf[i][j].delay, compact_pf[i][j].sl.size, compact_pf[i][j].cost);
			j++;
		}
		i++;
	}

}


path** compact_to_array(Dict_t** dist, int* nb_paths, int iter, int nbNodes, struct segment_list*** sl){
	path** compact_pf; 
	compact_pf = calloc(nbNodes+1, sizeof(path*));
	for (int i = 0 ; i < nbNodes ; i++) {
		compact_pf[i] = calloc(nb_paths[i]+1, sizeof(path));
		int index = 0;
		for (int seg = 0 ; seg < iter ; seg++) {
			for (int delay = 0 ; delay < dist[seg][i].size ; delay++) {
				if (dist[seg][i].paths[delay] != INF) {
					struct segment_list sl2 = sl[seg][i][delay]; 
					// printf("SIZE %d\n", sl[iter][i][delay].size);
					path p = {.cost =dist[seg][i].paths[delay], .delay=delay, .sl=sl2};
					compact_pf[i][index] = p;
					index++;
				}
			}
		}
		path end = {.cost = -1};
		compact_pf[i][nb_paths[i]] = end;
	}
	compact_pf[nbNodes] = NULL;
	return compact_pf;
}





// compact_front* compact_pfront(Dict_t** dist, int nbNodes, int iter){

// 	for (int i = 0 ; i < nbNodes ; i++) {
//         for (int delay = 0 ; delay < 1000 ; delay++) {
//         	compact_front[i] = compact_front_new(delay, );
// 			for (int seg = 0 ; seg < iter ; seg++) {
// 		        if (dist[seg][i].paths[delay] != INF) {
					
					
// 					// fprintf(output, "%d %d %d %d\n", i, j, dist[k][i].paths[j], k);
//                 }
//             }
//         }

//     }

// }



