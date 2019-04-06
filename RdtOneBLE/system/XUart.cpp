/*
 * XUart.cpp
 *
 * Created: 2019-03-30 12:18:00
 *  Author: kong
 */

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "system/XUtil.h"
#include "system/XUart.h"

ISR(USART0_RX_vect) {
    g_uart.rxIsr();
}

ISR(USART0_UDRE_vect) {
    g_uart.txIsr();
}

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
    uint16_t baudSetting = ((F_CPU / 8 / baudrate) - 1) / 2;
    outb(UBRR0H, baudSetting >> 8);
    outb(UBRR0L, baudSetting);
    sbi(UCSR0B, RXEN0);
    sbi(UCSR0B, TXEN0);
    sbi(UCSR0B, RXCIE0);
    sbi(UCSR0B, UDRIE0);
    m_rx_head = 0;
    m_rx_tail = 0;
    return 1;
}

void XUart::end()
{
    flush();
    cbi(UCSR0B, RXEN0);
    cbi(UCSR0B, TXEN0);
    cbi(UCSR0B, RXCIE0);
    cbi(UCSR0B, UDRIE0);
    m_rx_head = 0;
    m_rx_tail = 0;
}

int XUart::available()
{
    return ((int)(UART_BUF_SIZE + m_rx_head - m_rx_tail)) % UART_BUF_SIZE;
}

int XUart::peek()
{
    if (m_rx_head == m_rx_tail) {
        return -1;
        } else {
        return m_rx_buff[m_rx_tail];
    }
}

int XUart::read()
{
    if (m_rx_head == m_rx_tail) {
        return -1;
        } else {
        unsigned char c = m_rx_buff[m_rx_tail];
        m_rx_tail = (m_rx_tail + 1) % UART_BUF_SIZE;
        return c;
    }
}

size_t XUart::write(uint8_t data)
{
    m_tx_flag = true;
    if (m_tx_head == m_tx_tail && bit_is_set(UCSR0A, UDRE0)) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            outb(UDR0, data);
            outb(UCSR0A, bitval(UCSR0A, TXC0));
        }
        return 1;
    }
    uint8_t i = (m_tx_head + 1) % UART_BUF_SIZE;
    while (i == m_tx_tail) { // out buffer is full
        if (bit_is_clear(SREG, SREG_I)) {
            if (bit_is_set(UCSR0A, UDRE0)) txIsr();
        }
    }
    m_tx_buff[m_tx_head] = data;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m_tx_head = i;
        sbi(UCSR0B, UDRIE0);
    }
    return 1;
}

size_t XUart::write(const uint8_t* ptr, size_t length)
{
    size_t n = 0;
    while (length--) {
        if (write(*ptr++)) n++;
        else break;
    }
    return n;
}

size_t XUart::write(const char* str)
{
    if (str == NULL) return 0;
    return write((const uint8_t*)str, strlen(str));
}

void XUart::flush()
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

void XUart::rxIsr()
{
    if (bit_is_clear(UCSR0A, UPE0)) {
        unsigned char c = inb(UDR0);
        unsigned int  i = (m_rx_head + 1) % UART_BUF_SIZE;
        if (i != m_rx_tail) {
            m_rx_buff[m_rx_head] = c;
            m_rx_head = i;
        }
        } else {
        inb(UDR0); // parity error, read and discard it
    }
}

void XUart::txIsr()
{
    unsigned char c = m_tx_buff[m_tx_tail];
    m_tx_tail = (m_tx_tail + 1) % UART_BUF_SIZE;
    outb(UDR0, c);
    outb(UCSR0A, bitval(UCSR0A, TXC0));
    if (m_tx_head == m_tx_tail) {
        cbi(UCSR0B, UDRIE0);
    }
}

XUart g_uart;

/* EOF */