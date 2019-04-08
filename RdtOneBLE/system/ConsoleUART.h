/*
 * ConsoleUART.h
 *
 * Created: 2019-04-06 11:17:33
 *  Author: kong
 */ 

#ifndef CONSOLEUART_H_
#define CONSOLEUART_H_

#include <stdint.h>
#include <stdlib.h>

#define CONSOLE_BUF_SIZE 64

class ConsoleUART
{
public:
    ConsoleUART();
    ~ConsoleUART();
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
    unsigned char m_rx_buff[CONSOLE_BUF_SIZE];
    uint8_t m_tx_head;
    uint8_t m_tx_tail;
    unsigned char m_tx_buff[CONSOLE_BUF_SIZE];
};

extern ConsoleUART x_console;

#endif /* CONSOLEUART_H_ */