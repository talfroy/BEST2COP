#include "include/base.h"
#include "include/Topology.h"
#include "include/SrGraph.h"
#include "include/Dict.h"
#include "include/Best2cop.h"


#define MAX_IGP     100000000

//void Create_random_sr(int i,  int v_delay[], int v_igp[], char filename[]);


double get_mean(int tab[10][100], int size, int size2);

int main() {

    srand(time(NULL));
    double courbure = log(10.0) / log(9.0);
    double c_delay;
    INFO("END with delay\n");
    double c_igp = get_c(1, MAX_IGP, courbure);
    int spreads[19] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

    int v_igp[100000];
    int v_delay[100000];
    INFO("Start Tab initialization\n");
    fill_tab(c_igp, v_igp, courbure, 1, MAX_IGP);
    int iters[19][10][100];
    SrGraph_t* sr;
    Topology_t* topo;

    ParetoFront_t*** dist = NULL;
    Pfront_t** pfront = NULL;

    // for (int i = 0 ; i < 10000 ; i ++) {
    //     printf("%d\n", v_delay[i]);
    // }

    INFO("tabs successfully initialized\n");

    omp_set_num_threads(omp_get_max_threads());

    //printf("PRE_SPREAD SRC DST NB_ITERS\n");
    
    for (int i = 18 ; i < 19 ; i+= 2) {
        int spread = spreads[i];
        INFO("now with spread %d\n", spread);
        c_delay = get_c(1, spread, courbure);
        fill_tab(c_delay, v_delay, courbure, 1, spread);
        for (int n = 0 ; n < 1 ; n++) {
            //topo = Topology_create_random_quentin(1000, v_delay, v_igp, 10);
            topo = Topology_create_random_quentin(1000, v_delay, v_igp, 10);
            sr = SrGraph_create_from_topology_best_m2(topo);


            INFO("Topology %d with spread %d is loaded\n", n + 1, spread);
            int** iters2cop = malloc(1000 * sizeof(int*));
            for (int j = 0 ; j < 1000 ; j++) {
                iters2cop[j] = malloc(1000 * sizeof(int));
            }

            for (int j = 0 ; j < 100 ; j++) {
                dist = NULL;
                pfront = NULL;

                //iters[i][n][j] = Best2cop(&pfront, &dist, sr, j, 1000, INF, 1000, ANALYSE_2COP, &iters2cop[j]);
                //iters[i][n][j]--;
                //INFO("nb max iters = %d\n", iters[i][n][j]);

                for (int l = 0 ; l <= 100 ; l++) {
                    for (int k = 0 ; k < sr->nbNode ; k++) {
                        //Pfront_free(&pfront[l][k]);
                        //ParetoFront_free(dist[l][k]);
                    }
                    //free(pfront[l]);
                    //free(dist[l]);
                }

                // for (int m = 0 ; m < 1000 ; m++) {
                //     printf("%d %d %d %d\n", spread, j, m, iters2cop[j][m]);
                // }
                //free(pfront);
                //free(dist);
            }

            SrGraph_print_in_file(sr, stdout);
            Topology_print(topo, "test_topo_rand_zipf.isp");

            SrGraph_free(sr);
            Topology_free(topo);

            for (int j = 0 ; j < 1000 ; j++) {
                free(iters2cop[j]);
            }
            free(iters2cop);
        }


        //printf("%d %f\n", spread, get_mean(iters[i], 10, 100));
        //RESULTS("%d -> %f\n", spread, get_mean(iters[i], 10, 100));
        
    }


    return 0;
}


double get_mean(int tab[10][100], int size, int size2)
{
    double mean = 0;
    for (int i = 0 ; i < size ; i++) {
        for (int j = 0 ; j < size2 ; j++) {
            mean += tab[i][j];
        }
    }

    return mean / ((double)size * (double)size2);
}
