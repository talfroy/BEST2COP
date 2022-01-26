#include "../include/Topology.h"
#include "../include/base.h"
#include "../include/SrGraph.h"
#include "../include/Dict.h"
#define MAX_IGP 100000000
#define EVAL_NAME "DENSITY"
#define SIZE 1000
#define NB_SAMPLES 30
#define MAX_M1 100

int main()
{

    srand(time(NULL));
    // my_m1 MAX_M1 = atoi(argv[1]);

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

    INFO("tabs successfully initialized\n");
    char fileNameSR[30][128];
    char fileNameTopo[30][128];

    FILE **fileSR = malloc(NB_SAMPLES * sizeof(FILE *));
    FILE **fileTopo = malloc(NB_SAMPLES * sizeof(FILE *));

    SrGraph_t **sr = malloc(NB_SAMPLES * sizeof(SrGraph_t *));
    Topology_t **topo = malloc(NB_SAMPLES * sizeof(Topology_t *));

    omp_set_num_threads(omp_get_max_threads());

    for (int density = 1; density <= 100; density += 20)
    {
#pragma omp parallel for
        for (int j = 0; j < NB_SAMPLES; j++)
        {
            topo[j] = NULL;
            sr[j] = NULL;
            memset(fileNameSR[j], 0, 128);
            sprintf(fileNameSR[j], "%s/resultsSpread%d/topo%d/topoSR%d.isp", EVAL_NAME, MAX_M1, density, j);
            sprintf(fileNameTopo[j], "%s/resultsSpread%d/topo%d/topoRaw%d.isp", EVAL_NAME, MAX_M1, density, j);
            fileSR[j] = fopen(fileNameSR[j], "w");
            if (!fileSR[j]) {
                printf("Proble opening file SR\n");
            }
            fileTopo[j] = fopen(fileNameTopo[j], "w");
            if (!fileTopo[j]) {
                printf("Proble opening file Topo\n");
            }
            INFO("Start compute or spread %d %d nodes sample %d\n", MAX_M1, density, j);
            topo[j] = Topology_create_random_quentin(SIZE, v_delay, v_igp, density);
            sr[j] = SrGraph_create_from_topology_best_m2(topo[j]);

            while (SrGraph_get_max_spread(sr[j]) == -1)
            {
                INFO("There is a replay\n");
                Topology_free(topo[j]);
                topo[j] = Topology_create_random_quentin(SIZE, v_delay, v_igp, density);
                SrGraph_free(sr[j]);
                sr[j] = SrGraph_create_from_topology_best_m2(topo[j]);
            }

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
