#include "../include/Zipf.h"


int return_rank(float c, float courbure, float min, float max) {
    float sum_prob = 0.0;
    float ran = (float)RAND(0, 1000) / 1000.0;
    for (float i = min ; i <= max ; i++) {
        sum_prob += c / powf(i, courbure);
        if (sum_prob >= ran) {
            return (int)i;
        }
    }
    return -1;
}

float get_c(float min, float max, float courbure)
{
    float c = 0.0;
    for (float i = min ; i <= max ; i++) {
        c += (1.0 / powf((float)i, courbure));
    }
    return 1.0/c;
}


void fill_tab(float c, int tab[], float courbure, float min, float max)
{
    float sum_prob = 0.0;
    int index = 0;
    for (float i = min ; i <= max ; i++) {
        sum_prob += c / powf(i, courbure);
        while (sum_prob >= (float)index/10000.0) {
            tab[index] = (int)i;
            index++;
            if (index >= 10000) {
                return;
            }
        }
    }
}

