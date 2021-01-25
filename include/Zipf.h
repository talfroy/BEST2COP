#ifndef __ZIPF_H__
#define __ZIPF_H__

#include "base.h"

int return_rank(float c, float courbure, float min, float max);


/**
 * @brief return the value of c for the given parameters
 * 
 * @param min           minimal value of the distribution
 * @param max           maximal value of the distribution
 * @param courbure      zipf curve
 * 
 * @return return the c value
 */

float get_c(float min, float max, float courbure);


/**
 * @brief fill tab following a zipf distribution
 * To get an element from tab, you need to get an element 
 * between 0 and 1.  then, get the tab[rand * 10000] element
 * to get the value bound to this probability
 * 
 * @param c             c value
 * @param tab           tab to fill
 * @param courbure      zipf curve 
 * @param min           minimal value of the distribution
 * @param max           maximal value of the distribution        
 */

void fill_tab(float c, int tab[], float courbure, float min, float max);

#endif