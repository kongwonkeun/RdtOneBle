/*
 * RotationINTR.cpp
 *
 * Created: 2019-04-06 11:43:37
 *  Author: kong
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "system/Util.h"
#include "system/TickTIMER.h"
#include "system/ConsoleUART.h"
#include "system/BluetoothUART.h"
#include "system/RotationINTR.h"
#include "main.h"

ISR(INT0_vect)
{
    x_rotation.isr();
}

RotationINTR::RotationINTR() : m_rotation(0), m_msec(0), m_meter_per_sec(0)
{
}

RotationINTR::~RotationINTR()
{
}

int RotationINTR::begin()
{
    // rising edge interrupt
    sbi(EICRA, ISC01);
    sbi(EICRA, ISC00);
    sbi(EIMSK, INT0);
    return 1;
}

unsigned int RotationINTR::getVelocity()
{
    return m_meter_per_sec;
}

void RotationINTR::isr()
{
    m_rotation++;
    if (x_manual_mode) return;
    if (x_showup) x_console.write("R");
    
    unsigned long t = x_tick.getMillisec();
    if (m_msec > t) return; // ignore overflow
    m_meter_per_sec = (WHEEL_SIZE * 1000) / (t - m_msec);
    m_msec = t;
}

RotationINTR x_rotation;

/* EOF */