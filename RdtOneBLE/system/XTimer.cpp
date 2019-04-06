/*
 * XTimer.cpp
 *
 * Created: 2019-03-30 12:16:40
 *  Author: kong
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "system/XUtil.h"
#include "system/XConsole.h"
#include "system/XTimer.h"

ISR(TIMER0_COMP_vect) {
    g_timer0.isr();
}

XTimer::XTimer() : m_millisec(0), m_sec(0)
{
    //
}

XTimer::~XTimer()
{
    end(m_id);
}

void XTimer::begin(int id)
{
    if (id == 0) {
        sbi(TCCR0, CS02);  // prescaler(64)
        sbi(TCCR0, WGM01); // ctc mode
        outb(TCNT0, 0);
        outb(OCR0, 249);   // 1ms
        sbi(TIMSK, OCIE0); // interrupt enable
    }
}

void XTimer::end(int id)
{
    if (id == 0) {
        cbi(TIMSK, OCIE0); // disable
    }
}

unsigned long XTimer::millisec()
{
    return m_millisec;
}

unsigned long XTimer::sec()
{
    return m_sec;
}

void XTimer::isr()
{
    m_millisec++;
    if (!(m_millisec % 1000)) {
        m_sec++;
        g_console.write("T");
    }
}

XTimer g_timer0;

/* EOF */