/*
 * XConsole.h
 *
 * Created: 2019-04-03 18:24:20
 *  Author: kong
 */

#ifndef XCONSOLE_H_
#define XCONSOLE_H_

#include <stdint.h>
#include <stdlib.h>

#define CONSOLE_BUF_SIZE 64

class XConsole
{
public:
    XConsole();
    virtual ~XConsole();
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
    bool m_tx_flag;
};

extern XConsole g_console;

#endif /* XCONSOLE_H_ */