#include "../include/Zipf.h"


int return_rank(double c, double courbure, double min, double max) {
    double sum_prob = 0.0;
    double ran = (double)RAND(0, 1000) / 1000.0;
    for (double i = min ; i <= max ; i++) {
        sum_prob += c / powf(i, courbure);
        if (sum_prob >= ran) {
            return (int)i;
        }
    }
    return -1;
}

double get_c(double min, double max, double courbure)
{
    double c = 0.0;
    for (double i = min ; i <= max ; i++) {
        c += (1.0 / powf((double)i, courbure));
        if ((int)i % 1000 == 0) {
            //INFO("i = %.1f\n", i);
        }
    }
    return 1.0/c;
}


void fill_tab(double c, int tab[], double courbure, double min, double max)
{
    double sum_prob = 0.0;
    int index = 0;
    for (double i = min ; i <= max ; i++) {
        sum_prob += c / powf(i, courbure);
        while (sum_prob >= (double)index/100000.0) {
            tab[index] = (int)i;
            index++;
            if (index >= 100000) {
                return;
            }
        }
    }
}

