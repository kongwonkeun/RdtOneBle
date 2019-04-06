﻿/*
 * XUart.h
 *
 * Created: 2019-03-27 13:06:53
 *  Author: kong
 */

#ifndef XUART_H_
#define XUART_H_

#include <stdint.h>
#include <stdlib.h>

#define UART_BUF_SIZE 64

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
    size_t write(uint8_t data);
    size_t write(const uint8_t* ptr, size_t length);
    size_t write(const char* str);
    void flush();
    void rxIsr();
    void txIsr();
private:
    uint8_t m_rx_head;
    uint8_t m_rx_tail;
    unsigned char m_rx_buff[UART_BUF_SIZE];
    uint8_t m_tx_head;
    uint8_t m_tx_tail;
    unsigned char m_tx_buff[UART_BUF_SIZE];
    bool m_tx_flag;
};

extern XUart g_uart;

#endif /* XUART_H_ */