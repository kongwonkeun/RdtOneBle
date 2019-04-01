/*
 * XUart.cpp
 *
 * Created: 2019-03-30 12:18:00
 *  Author: kong
 */

#include "system/XUart.h"

XUart::XUart()
{
    //
}

XUart::~XUart()
{
    //
}

int XUart::begin(unsigned long baudrate)
{
    return 0;
}

void XUart::end()
{
    return;
}

int XUart::available()
{
    return 0;
}

int XUart::peek()
{
    return 0;
}

int XUart::read()
{
    return 0;
}

size_t XUart::write(const uint8_t* data, size_t length)
{
    return 0;
}

void XUart::flush()
{
    return;
}

XUart g_uart;

/* EOF */