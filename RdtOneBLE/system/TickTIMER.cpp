/*
 * TickTIMER.cpp
 *
 * Created: 2019-04-06 11:28:57
 *  Author: kong
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "system/Util.h"
#include "system/ConsoleUART.h"
#include "system/BluetoothUART.h"
#include "system/RotationINTR.h"
#include "system/DistanceADC.h"
#include "system/TickTIMER.h"
#include "main.h"

ISR(TIMER0_COMP_vect) {
    x_tick.isr();
}

TickTIMER::TickTIMER() : m_millisec(0), m_sec(0)
{
}

TickTIMER::~TickTIMER()
{
    end();
}

void TickTIMER::begin(unsigned int interval)
{
    m_interval = interval;
    sbi(TCCR0, CS02);  // prescaler(64)
    sbi(TCCR0, WGM01); // ctc mode
    outb(TCNT0, 0);
    outb(OCR0, 249);   // 1ms
    sbi(TIMSK, OCIE0); // interrupt enable
}

void TickTIMER::end()
{
    cbi(TIMSK, OCIE0); // disable
}

unsigned long TickTIMER::getMillisec()
{
    return m_millisec;
}

unsigned long TickTIMER::getSec()
{
    return m_sec;
}

void TickTIMER::incInterval()
{
    m_interval++;
    char str[10];
    wordToString(m_interval, str);
    x_console.write("\ninterval is ");
    x_console.write(str);
    x_console.write("\n");
}

void TickTIMER::decInterval()
{
    if (m_interval < 2) {
        x_console.write("\ninterval must be greater than 1\n");
        return;
    }
    m_interval--;
    char str[10];
    wordToString(m_interval, str);
    x_console.write("\ninterval is ");
    x_console.write(str);
    x_console.write("\n");
}

void TickTIMER::isr()
{
    m_millisec++;
    if (x_manual_mode) return;
    if (x_showup) x_console.write("T");

    if (!(m_millisec % m_interval)) { // 20msec tick
        unsigned char d;
        char s[10];
        
        d = (unsigned char)(x_rotation.getVelocity());
        byteToString('V', d, s);
        x_bluetooth.write(s);
        
        d = (unsigned char)(x_distance.getDistance());
        byteToString('D', d, s);
        x_bluetooth.write(s);
    }
}

TickTIMER x_tick;

/* EOF */