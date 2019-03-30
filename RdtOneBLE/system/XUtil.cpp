/*
 * XUtil.cpp
 *
 * Created: 2019-03-30 13:28:02
 *  Author: kong
 */ 


#include "../system/XUtil.h"


void* operator new(size_t n)
{
    void* const p = malloc(n);
    return p;
}

void operator delete(void* p)
{
    free(p);
}

