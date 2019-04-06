/*
 * XConsole.cpp
 *
 * Created: 2019-04-03 18:25:01
 *  Author: kong
 */

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "system/XUtil.h"
#include "system/XConsole.h"

ISR(USART1_RX_vect)
{
    g_console.rxIsr();
}

ISR(USART1_UDRE_vect)
{
    g_console.txIsr();
}

XConsole::XConsole()
{
    //
}

XConsole::~XConsole()
{
    //
}

int XConsole::begin(unsigned long baudrate)
{
    uint16_t baudSetting = ((F_CPU / 8 / baudrate) - 1) / 2;
    outb(UBRR1H, baudSetting >> 8);
    outb(UBRR1L, baudSetting);
    sbi(UCSR1B, RXEN1);
    sbi(UCSR1B, TXEN1);
    sbi(UCSR1B, RXCIE1);
    sbi(UCSR1B, UDRIE1);
    m_rx_head = 0;
    m_rx_tail = 0;
    return 1;
}

void XConsole::end()
{
    flush();
    cbi(UCSR1B, RXEN1);
    cbi(UCSR1B, TXEN1);
    cbi(UCSR1B, RXCIE1);
    cbi(UCSR1B, UDRIE1);
    m_rx_head = 0;
    m_rx_tail = 0;
}

int XConsole::available()
{
    return ((int)(CONSOLE_BUF_SIZE + m_rx_head - m_rx_tail)) % CONSOLE_BUF_SIZE;
}

int XConsole::peek()
{
    if (m_rx_head == m_rx_tail) {
        return -1;
    } else {
        return m_rx_buff[m_rx_tail];
    }    
}

int XConsole::read()
{
    if (m_rx_head == m_rx_tail) {
        return -1;
    } else {
        unsigned char c = m_rx_buff[m_rx_tail];
        m_rx_tail = (m_rx_tail + 1) % CONSOLE_BUF_SIZE;
        return c;
    }
}

size_t XConsole::write(uint8_t data)
{
    m_tx_flag = true;
    if (m_tx_head == m_tx_tail && bit_is_set(UCSR1A, UDRE1)) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            outb(UDR1, data);
            outb(UCSR1A, bitval(UCSR1A, TXC1));
        }
        return 1;
    }
    uint8_t i = (m_tx_head + 1) % CONSOLE_BUF_SIZE;
    while (i == m_tx_tail) { // out buffer is full
        if (bit_is_clear(SREG, SREG_I)) {
            if (bit_is_set(UCSR1A, UDRE1)) txIsr();
        }
    }
    m_tx_buff[m_tx_head] = data;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m_tx_head = i;
        sbi(UCSR1B, UDRIE1);
    }
    return 1;
}

size_t XConsole::write(const uint8_t* ptr, size_t length)
{
    size_t n = 0;
    while (length--) {
        if (write(*ptr++)) n++;
        else break;
    }
    return n;
}

size_t XConsole::write(const char* str)
{
    if (str == NULL) return 0;
    return write((const uint8_t*)str, strlen(str));
}

void XConsole::flush()
{
    if (!m_tx_flag) return;
    while (bit_is_set(UCSR1B, UDRIE1) || bit_is_clear(UCSR1A, TXC1)) {
        if (bit_is_clear(SREG, SREG_I) && bit_is_set(UCSR1B, UDRIE1)) {
            if (bit_is_set(UCSR1A, UDRE1)) {
                txIsr();
            }
        }
    }
}

void XConsole::rxIsr()
{
    if (bit_is_clear(UCSR1A, UPE1)) {
        unsigned char c = inb(UDR1);
        unsigned int  i = (m_rx_head + 1) % CONSOLE_BUF_SIZE;
        if (i != m_rx_tail) {
            m_rx_buff[m_rx_head] = c;
            m_rx_head = i;
        }
    } else {
        inb(UDR1); // parity error, read and discard it
    }
}

void XConsole::txIsr()
{
    unsigned char c = m_tx_buff[m_tx_tail];
    m_tx_tail = (m_tx_tail + 1) % CONSOLE_BUF_SIZE;
    outb(UDR1, c);
    outb(UCSR1A, bitval(UCSR1A, TXC1));
    if (m_tx_head == m_tx_tail) {
        cbi(UCSR1B, UDRIE1);
    }
}

XConsole g_console;

/* EOF */