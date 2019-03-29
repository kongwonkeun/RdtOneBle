﻿/*
 * XUtil.h
 *
 * Created: 2019-03-28 10:51:30
 *  Author: kong
 */


#ifndef XUTIL_H_
#define XUTIL_H_

#include <stdint.h>

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef boolean
typedef bool boolean;
#endif

#ifndef byte
typedef uint8_t byte;
#endif

#ifndef word
typedef uint16_t word;
#endif


#endif /* XUTIL_H_ */