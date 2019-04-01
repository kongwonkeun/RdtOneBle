/*
 * HCITransport.h
 *
 * Created: 2019-03-27 12:22:25
 *  Author: kong
 */

#ifndef HCITRANSPORT_H_
#define HCITRANSPORT_H_

#include <stdint.h>
#include <stdlib.h>

class HCITransportInterface
{
public:
    virtual int  begin() = 0;
    virtual void end() = 0;
    virtual void wait(unsigned long timeout) = 0;
    virtual int  available() = 0;
    virtual int  peek() = 0;
    virtual int  read() = 0;
    virtual size_t write(const uint8_t* data, size_t length) = 0;
private:
};

extern HCITransportInterface& g_hciTransport;

#endif /* HCITRANSPORT_H_ */