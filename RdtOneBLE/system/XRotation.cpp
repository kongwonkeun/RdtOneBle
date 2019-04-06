/*
 * XRotation.cpp
 *
 * Created: 2019-04-04 17:41:47
 *  Author: kong
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "system/XUtil.h"
#include "system/XConsole.h"
#include "system/XRotation.h"

ISR(INT0_vect)
{
    g_rotation.isr();
}

XRotation::XRotation()
{
    //
}

XRotation::~XRotation()
{
    //
}

int XRotation::begin()
{
    // rising edge interrupt
    sbi(EICRA, ISC01);
    sbi(EICRA, ISC00);
    sbi(EIMSK, INT0);
    return 1;
}

void XRotation::isr()
{
    m_rotation++;
    g_console.write("R");
}

XRotation g_rotation;

/* EOF */