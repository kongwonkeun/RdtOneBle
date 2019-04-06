/*
 * XUtil.cpp
 *
 * Created: 2019-03-30 13:28:02
 *  Author: kong
 */

#include "system/XUtil.h"

void* operator new(size_t n)
{
    void* const p = malloc(n);
    return p;
}

void operator delete(void* p)
{
    free(p);
}

unsigned int map(
unsigned int x, unsigned int x_min, unsigned int x_max, 
unsigned int y_min, unsigned int y_max)
{
    unsigned int y = ((x - x_min) * (y_max - y_max) / (x_max - x_min)) + y_min;
    return y;
}

/* EOF */