/*
 * HCIUartTransport.h
 *
 * Created: 2019-03-27 12:38:21
 *  Author: kong
 */

#ifndef HCIUARTTRANSPORT_H_
#define HCIUARTTRANSPORT_H_

#include <stdint.h>
#include <stdlib.h>
#include "protocol/HCITransport.h"

class XUart;

class HCIUartTransportClass : public HCITransportInterface
{
public:
    HCIUartTransportClass(XUart& uart, unsigned long baudrate);
    virtual ~HCIUartTransportClass();
    virtual int  begin();
    virtual void end();
    virtual void wait(unsigned long timeout);
    virtual int  available();
    virtual int  peek();
    virtual int  read();
    virtual size_t write(const uint8_t* data, size_t length);
private:
    XUart* m_uart;
    unsigned long m_baudrate;
};

#endif /* HCIUARTTRANSPORT_H_ */