/*
 * HCIUartTransport.cpp
 *
 * Created: 2019-03-29 13:49:42
 *  Author: kong
 */

#include "system/XUart.h"
#include "system/XTimer.h"
#include "protocol/HCIUartTransport.h"

HCIUartTransportClass::HCIUartTransportClass(XUart& uart, unsigned long baudrate) : m_uart(&uart), m_baudrate(baudrate)
{
    // just initialize properties
}

HCIUartTransportClass::~HCIUartTransportClass()
{
    // nothing to do
}

int HCIUartTransportClass::begin()
{
    m_uart->begin(m_baudrate);
    return 1;
}

void HCIUartTransportClass::end()
{
    m_uart->end();
}

void HCIUartTransportClass::wait(unsigned long timeout)
{
    for (unsigned long start = g_timer0.millisec(); (g_timer0.millisec() - start) < timeout;) {
        if (available()) {
            break;
        }
    }
}

int HCIUartTransportClass::available()
{
    return m_uart->available();
}

int HCIUartTransportClass::peek()
{
    return m_uart->peek();
}

int HCIUartTransportClass::read()
{
    return m_uart->read();
}

size_t HCIUartTransportClass::write(const uint8_t* data, size_t length)
{
    size_t result = m_uart->write(data, length);
    m_uart->flush();
    return result;
}

HCIUartTransportClass  g_hciUartTransport(g_uart, 115200);
HCITransportInterface& g_hciTransport = g_hciUartTransport;

/* EOF */