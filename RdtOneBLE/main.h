/*
 * main.h
 *
 * Created: 2019-04-03 16:52:01
 *  Author: kong
 */ 

#ifndef MAIN_H_
#define MAIN_H_

//#define F_CPU 16000000UL // move to compile option

#define BAUDRATE_CONSOLE 115200UL
#define BAUDRATE_BLUETOOTH 115200UL
#define TICK_INTERVAL 20 // 20 millisec

extern bool x_showup;
extern bool x_manual_mode;

#endif /* MAIN_H_ */