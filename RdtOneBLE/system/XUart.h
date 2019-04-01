/*
 * XUart.h
 *
 * Created: 2019-03-27 13:06:53
 *  Author: kong
 */

#ifndef XUART_H_
#define XUART_H_

#include <stdint.h>
#include <stdlib.h>

class XUart
{
public:
    XUart();
    virtual ~XUart();
    int  begin(unsigned long baudrate);
    void end();
    int  available();
    int  peek();
    int  read();
    size_t write(const uint8_t* data, size_t length);
    void flush();
private:
};

extern XUart g_uart;

#endif /* XUART_H_ */