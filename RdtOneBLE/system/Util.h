/*
 * Util.h
 *
 * Created: 2019-04-06 10:50:39
 *  Author: kong
 */ 

#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

#define BAUDRATE_CONFIG_VALUE(freq,baud) (((float)(freq))/(((float)(baud))*16.0)-1.0+0.5)

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
unsigned long x, unsigned long x_min, unsigned long x_max,
unsigned long y_min, unsigned long y_max
);
void  byteToString(char key, unsigned char data, char *str);
void  wordToString(char key, unsigned int data,  char *str);
void  byteToString(unsigned char data, char *str);
void  wordToString(unsigned int data,  char *str);

#define bit_is_set(sfr,bit) (_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr,bit) (!(_SFR_BYTE(sfr) & _BV(bit)))

#define bitval(port,bit) ((port) & _BV(bit))
#define outb(port,val) ((port) = (val))
#define inb(port) (port)

#define sbi(port,bit) ((port) |=  (1 << (bit)))
#define cbi(port,bit) ((port) &= ~(1 << (bit)))

#endif /* UTIL_H_ */