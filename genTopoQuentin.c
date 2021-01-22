#include "include/base.h"
#include "include/Topology.h"
#include "include/SrGraph.h"
#include "include/Dict.h"
#include "include/Best2cop.h"


#define MAX_IGP 10000000


int main(int argc, char** argv) {

    if (argc != 3) {
        return 1;
    }

    my_m1 max_m1 = atoi(argv[1]);
    int nb_sample = atoi(argv[2]);

    srand(time(NULL));
    float courbure = log(5.0) / log(4.0);
    float c_delay = get_c(1, max_m1/10, courbure);
    INFO("END with delay\n");
    float c_igp = get_c(1, MAX_IGP, courbure);

    int v_igp[10000];
    memset(v_igp, 0, 10000 * sizeof(int));
    int v_delay[10000];
    memset(v_igp, 0, 10000 * sizeof(int));
    INFO("Start Tab initialization\n");
    fill_tab(c_igp, v_igp, courbure, 1, MAX_IGP);
    fill_tab(c_delay, v_delay, courbure, 1, max_m1/10);

    // for (int i = 0 ; i < 10000 ; i ++) {
    //     printf("%d\n", v_delay[i]);
    // }

    INFO("tabs successfully initialized\n");
    char fileName[30][128];

    
    FILE** file = malloc(nb_sample * sizeof(FILE*));
    SrGraph_t** sr = malloc(nb_sample * sizeof(SrGraph_t*));
    Topology_t** topo = malloc(nb_sample * sizeof(Topology_t*));


    omp_set_num_threads(omp_get_max_threads());
    

    for (int i = 1000 ; i <= 1000 ; i += 100) {
        //#pragma omp parallel for
        for (int j = 0 ; j < nb_sample ; j++) {
            topo[j] = NULL;
            sr[j] = NULL;
            memset(fileName[j], 0, 128);
            sprintf(fileName[j], "RND/resultsSpread%d/topo%d/sr_topo%d.isp", max_m1, i, j);
            file[j] = fopen(fileName[j], "w");

            INFO("Start compute or spread %d %d nodes sample %d\n", max_m1, i, j);
            topo[j] = Topology_create_random_pascal(i, v_delay, v_igp, 10);
            Topology_print(topo[j], "testRandomTopo50_1000.isp");
            sr[j] = SrGraph_create_from_topology_best_m2(topo[j]);
            if (SrGraph_get_max_spread(sr[j]) != -1) {
                INFO("C'est ok\n");
                SrGraph_print_in_file(sr[j], file[j]);
            } else {
                ERROR("Must be a connexe component\n");
            }
            //SrGraph_check_m1(sr[j]);
            
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