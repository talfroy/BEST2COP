#include <stdlib.h>
#include <stdio.h>

#define MAX(a,b) ((a > b) ? a : b)

double myabs(double d)
{
    if (d < 0.0) {
        return -d;
    }

    return d;
}

int main(int argc, char** argv)
{
    FILE* sprint = fopen(argv[1], "r");
    FILE* random = fopen(argv[2], "r");

    double max_rand = 0;
    double max_spri = 0;
    double max_del_rand = 0;
    double max_del_spri = 0;

    char line[1024];
    char srcLabel[128];
    char destLabel[128];
    double m1, m2;

    double mean_prop_spri = 0.0;
    double mean_prop_rand = 0.0;

    int line_spri = 1;
    int line_rand = 1;


    while (fgets(line, 1024,  sprint))
    {
        if (sscanf(line, "%s %s %lf %lf\n", &srcLabel[0], &destLabel[0], &m1, &m2) == 4) {
            max_spri = MAX(max_spri, m2);
            max_del_spri = MAX(max_del_spri, m1);
        }
        line_spri++;
    }

    fclose(sprint);


    while (fgets(line, 1024,  random))
    {
        if (sscanf(line, "%s %s %lf %lf\n", &srcLabel[0], &destLabel[0], &m1, &m2) == 4) {
            max_rand = MAX(max_rand, m2);
            max_del_rand = MAX(max_del_rand, m1);
        }
        line_rand++;
    }

    fclose(random);

    sprint = fopen(argv[1], "r");
    random = fopen(argv[2], "r");


    while (fgets(line, 1024,  sprint))
    {
        if (sscanf(line, "%s %s %lf %lf\n", &srcLabel[0], &destLabel[0], &m1, &m2) == 4) {
            mean_prop_spri += myabs((m2 / max_spri) - (m1 / max_del_spri));
        }
    }


    while (fgets(line, 1024,  random))
    {
        if (sscanf(line, "%s %s %lf %lf\n", &srcLabel[0], &destLabel[0], &m1, &m2) == 4) {
            mean_prop_rand += myabs((m2 / max_rand) - (m1 / max_del_rand));
            //printf("%lf, %lf\n", m2 / max_rand, m1 / max_del_rand);
        }
    }

    mean_prop_spri /= (double)line_spri;
    mean_prop_rand /= (double)line_rand;

    printf("sprint : %lf, rand : %lf\n", mean_prop_spri, mean_prop_rand);

    return 0;
}