#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


int rand_a_b(int a, int b) {
    return rand()%(b-a) + a;
}

int return_rank(float c, float courbure) {
    float sum_prob = 0.0;
    float rand = (float)rand_a_b(0, 1000) / 1000.0;
    printf("rand = %f\n", rand);
    for (float i = 1 ; i <= 1000 ; i++) {
        sum_prob += c / powf(i, courbure);
        if (sum_prob >= rand) {
            return (int)i;
        }
    }
    printf("Y a un soucis\n");
}

int get_rank_not_rand(float rank, float c, float courbure) {
    float sum_prob = 0.0;
    for (float i = 1 ; i <= 1000 ; i++) {
        sum_prob += c / powf(i, courbure);
        if (sum_prob >= rank) {
            return (int)i;
        }
    }
}

int main(int argc, char** argv) {

    srand(time(NULL));
    int rands[100000];
    float courbure = log(5.0) / log(4.0);
    float c = 0.0;
    int actindex = 0;

    for (float i = 1 ; i <= 1000 ; i++) {
        c += (1.0 / powf(i, courbure));
    }

    c = 1.0 / c;

    FILE* file = fopen(argv[1], "r");
    char filename[128];
    sprintf(filename, "sprint-modif-zipf.isp");
    FILE* output = fopen(filename, "w");
    char line[1024];
    char src[128];
    char dst[128];
    int igp;
    float delay;

    while(fgets(line, 1024, file))
    {
        if (sscanf(line, "%s %s %f %d\n", &src[0], &dst[0], &delay, &igp))
        {
            delay = (float)return_rank(c, courbure) / 10.0;
            fprintf(output, "%s %s %.2f %d\n", src, dst, delay, igp);
        } else {
		    printf("pas pu lire\n");
	    }
    }

    return 0;
}