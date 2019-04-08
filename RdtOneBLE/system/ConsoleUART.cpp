/*
 * ConsoleUART.cpp
 *
 * Created: 2019-04-06 11:19:41
 *  Author: kong
 */

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "system/Util.h"
#include "system/BluetoothUART.h"
#include "system/ConsoleUART.h"
#include "main.h"

ISR(USART1_RX_vect)
{
    x_console.rxIsr();
}

ISR(USART1_UDRE_vect)
{
    x_console.txIsr();
}

ISR(USART1_TX_vect)
{
    x_console.txIsr();
}

ConsoleUART::ConsoleUART() : 
m_rx_head(0), m_rx_tail(0), m_tx_head(0), m_tx_tail(0)
{
}

ConsoleUART::~ConsoleUART()
{
}

int ConsoleUART::begin(unsigned long baudrate)
{
    uint16_t baudSetting = BAUDRATE_CONFIG_VALUE(F_CPU,baudrate);
    outb(UBRR1H, baudSetting >> 8);
    outb(UBRR1L, baudSetting);
    sbi(UCSR1B, RXEN1);
    sbi(UCSR1B, TXEN1);
    sbi(UCSR1B, RXCIE1);
    sbi(UCSR1B, UDRIE1);
    //sbi(UCSR1B, TXCIE1);
    return 1;
}

void ConsoleUART::end()
{
    flush();
    cbi(UCSR1B, RXEN1);
    cbi(UCSR1B, TXEN1);
    cbi(UCSR1B, RXCIE1);
    cbi(UCSR1B, UDRIE1);
    //cbi(UCSR1B, TXCIE1);
}

int ConsoleUART::available()
{
    return ((int)(CONSOLE_BUF_SIZE + m_rx_head - m_rx_tail)) % CONSOLE_BUF_SIZE;
}

int ConsoleUART::peek()
{
    if (m_rx_head == m_rx_tail) {
        return -1;
    } else {
        return m_rx_buff[m_rx_tail];
    }
}

int ConsoleUART::read()
{
    if (m_rx_tail == m_rx_head) { // rx buffer is empty
        return -1;
    } else {
        unsigned char c = m_rx_buff[m_rx_tail];
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            m_rx_tail = (m_rx_tail + 1) % CONSOLE_BUF_SIZE;
        }        
        return c;
    }
}

size_t ConsoleUART::write(uint8_t data)
{
    uint8_t i = (m_tx_head + 1) % CONSOLE_BUF_SIZE;
    while (i == m_tx_tail) { // tx buffer full
        sbi(UCSR1B, UDRIE1);
    }
    m_tx_buff[m_tx_head] = data;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m_tx_head = (m_tx_head + 1) % CONSOLE_BUF_SIZE;
        sbi(UCSR1B, UDRIE1);
    }
    return 1;
}

size_t ConsoleUART::write(const uint8_t* ptr, size_t length)
{
    size_t n = 0;
    while (length--) {
        if (write(*ptr++)) n++;
        else break;
    }
    return n;
}

size_t ConsoleUART::write(const char* str)
{
    if (str == NULL) return 0;
    return write((const uint8_t*)str, strlen(str));
}

void ConsoleUART::flush()
{
    while (bit_is_set(UCSR1B, UDRIE1) || bit_is_clear(UCSR1A, TXC1)) {
        if (bit_is_clear(SREG, SREG_I) && bit_is_set(UCSR1B, UDRIE1)) {
            if (bit_is_set(UCSR1A, UDRE1)) {
                txIsr();
            }
        }
    }
}

void ConsoleUART::rxIsr()
{
    unsigned char c = inb(UDR1);
    uint8_t i = (m_rx_head + 1) % CONSOLE_BUF_SIZE;
    if (i != m_rx_tail) {
        m_rx_buff[m_rx_head] = c;
        m_rx_head = i;
    }
    x_console.write((uint8_t)c); // echo
    if (x_manual_mode) {
        x_bluetooth.write((uint8_t)c); // send to remote
    }
}

void ConsoleUART::txIsr()
{
    if (m_tx_tail == m_tx_head) { // buffer is empty
        cbi(UCSR1B, UDRIE1); // disable interrupt
    }
    else {
        unsigned char c = m_tx_buff[m_tx_tail];
        m_tx_tail = (m_tx_tail + 1) % CONSOLE_BUF_SIZE;
        outb(UDR1, c);
    }
}

ConsoleUART x_console;

/* EOF */