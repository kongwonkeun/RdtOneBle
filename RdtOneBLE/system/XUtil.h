/*
 * XUtil.h
 *
 * Created: 2019-03-28 10:51:30
 *  Author: kong
 */

#ifndef XUTIL_H_
#define XUTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

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

void* operator new(size_t n);
void  operator delete(void* p);
unsigned int map(
    unsigned int x, unsigned int x_min, unsigned int x_max, 
    unsigned int y_min, unsigned int y_max
);

#ifndef bit_is_set
#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
#endif

#ifndef bit_is_clear
#define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
#endif

#define bitval(port, bit) ((port) & _BV(bit))
#define outb(port, val) ((port) = (val))
#define inb(port) (port)

#ifndef sbi
#define sbi(port, bit) ((port) |= (1 << (bit)))
#endif

#ifndef cbi
#define cbi(port, bit) ((port) &= ~(1 << (bit)))
#endif

#endif /* XUTIL_H_ */