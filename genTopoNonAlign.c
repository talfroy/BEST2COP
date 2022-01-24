#include "include/base.h"
#include "include/Topology.h"
#include "include/SrGraph.h"
#include "include/Dict.h"
#include "include/Best2cop.h"

#define MAX_IGP 100000000

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        return 1;
    }

    srand(time(NULL));
    my_m1 max_m1 = atoi(argv[1]);
    int nb_sample = atoi(argv[2]);

    double courbure = log(10.0) / log(9.0);
    double c_delay;
    INFO("END with delay\n");
    double c_igp = get_c(1, MAX_IGP, courbure);

    int v_igp[100000];
    int v_delay[100000];
    INFO("Start Tab initialization\n");
    fill_tab(c_igp, v_igp, courbure, 1, MAX_IGP);

    int spread = 1000;
    c_delay = get_c(1, spread, courbure);
    fill_tab(c_delay, v_delay, courbure, 1, spread);

    // for (int i = 0 ; i < 10000 ; i ++) {
    //     printf("%d\n", v_delay[i]);
    // }

    INFO("tabs successfully initialized\n");
    char fileNameSR[30][128];
    char fileNameTopo[30][128];

    FILE **fileSR = malloc(nb_sample * sizeof(FILE *));
    FILE **fileTopo = malloc(nb_sample * sizeof(FILE *));

    SrGraph_t **sr = malloc(nb_sample * sizeof(SrGraph_t *));
    Topology_t **topo = malloc(nb_sample * sizeof(Topology_t *));

    omp_set_num_threads(omp_get_max_threads());

    for (int i = 100; i <= 1000; i += 100)
    {
#pragma omp parallel for
        for (int j = 0; j < nb_sample; j++)
        {
            topo[j] = NULL;
            sr[j] = NULL;
            memset(fileNameSR[j], 0, 128);
            sprintf(fileNameSR[j], "RND/resultsSpread%d/topo%d/topoSR%d.isp", max_m1, i, j);
            sprintf(fileNameTopo[j], "RND/resultsSpread%d/topo%d/topoRaw%d.isp", max_m1, i, j);
            fileSR[j] = fopen(fileNameSR[j], "w");
            fileTopo[j] = fopen(fileNameTopo[j], "w");
            INFO("Start compute or spread %d %d nodes sample %d\n", max_m1, i, j);
            topo[j] = Topology_create_random_quentin(i, v_delay, v_igp, 10);
            sr[j] = SrGraph_create_from_topology_best_m2(topo[j]);
            while (SrGraph_get_max_spread(sr[j]) == -1)
            {
                INFO("There is a replay\n");
                Topology_free(topo[j]);
                topo[j] = Topology_create_random_quentin(i, v_delay, v_igp, 10);
                SrGraph_free(sr[j]);
                sr[j] = SrGraph_create_from_topology_best_m2(topo[j]);
            }
            //SrGraph_check_m1(sr[j]);
            SrGraph_print_in_file(sr[j], fileSR[j]);
            Topology_print(topo[j], fileNameTopo[j]);
            fclose(fileTopo[j]);
            fclose(fileSR[j]);
            SrGraph_free(sr[j]);
            Topology_free(topo[j]);
            printf("File %s done\n", fileNameSR[j]);
        }
    }

    free(fileSR);
    free(fileTopo);
    free(sr);
    free(topo);
    return 0;
}
