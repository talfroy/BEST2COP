#ifndef __ZIPF_H__
#define __ZIPF_H__

#include "base.h"

int return_rank(float c, float courbure, float min, float max);

float get_c(float min, float max, float courbure);

void fill_tab(float c, int tab[], float courbure, float min, float max);

#endif