#include "include/base.h"
#include "include/Topology.h"
#include "include/SrGraph.h"
#include "include/Dict.h"
#include "include/Best2cop.h"


#define MAX_IGP INF/50


int main(int argc, char** argv) {

    if (argc != 3) {
        return 1;
    }

    my_m1 max_m1 = atoi(argv[1]);
    int nb_sample = atoi(argv[2]);

    srand(time(NULL));

    // for (int i = 0 ; i < 10000 ; i ++) {
    //     printf("%d\n", v_delay[i]);
    // }

    INFO("tabs successfully initialized\n");
    char fileName[30][128];

    
    FILE** file = malloc(nb_sample * sizeof(FILE*));
    SrGraph_t** sr = malloc(nb_sample * sizeof(SrGraph_t*));
    Topology_t** topo = malloc(nb_sample * sizeof(Topology_t*));


    omp_set_num_threads(omp_get_max_threads());
    

    for (int i = 100 ; i <= 1000 ; i += 100) {
        #pragma omp parallel for
        for (int j = 0 ; j < nb_sample ; j++) {
            topo[j] = NULL;
            sr[j] = NULL;
            memset(fileName[j], 0, 128);
            sprintf(fileName[j], "RND/resultsSpread%d/topo%d/sr_topo%d.isp", max_m1, i, j);
            file[j] = fopen(fileName[j], "w");

            INFO("Start compute or spread %d %d nodes sample %d\n", max_m1, i, j);
            topo[j] = Topology_create_random_non_align(i, 10, max_m1, MAX_IGP);
            sr[j] = SrGraph_create_from_topology_best_m2(topo[j]);
            while (!SrGraph_is_connex(sr[j])) {
                INFO("There is a replay\n");
                Topology_free(topo[j]);
                topo[j] = Topology_create_random_non_align(i, 10, max_m1, MAX_IGP);
                SrGraph_free(sr[j]);
                sr[j] = SrGraph_create_from_topology_best_m2(topo[j]);
            }
            //SrGraph_check_m1(sr[j]);

            SrGraph_print_in_file(sr[j], file[j]);
            
            fclose(file[j]);
            SrGraph_free(sr[j]);
            Topology_free(topo[j]);
            printf("File %s done\n", fileName[j]);
        }
    }

    free(file);
    free(sr);
    free(topo);
    return 0;
}