/*
 * BluetoothUART.cpp
 *
 * Created: 2019-04-06 11:09:18
 *  Author: kong
 */

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "system/Util.h"
#include "system/ConsoleUART.h"
#include "system/BluetoothUART.h"
#include "main.h"

ISR(USART0_RX_vect) {
    x_bluetooth.rxIsr();
}

ISR(USART0_UDRE_vect) {
    x_bluetooth.txIsr();
}

ISR(USART0_TX_vect)
{
    x_bluetooth.txIsr();
}

BluetoothUART::BluetoothUART() : 
m_rx_head(0), m_rx_tail(0), m_tx_head(0), m_tx_tail(0)
{
}

BluetoothUART::~BluetoothUART()
{
}

int BluetoothUART::begin(unsigned long baudrate)
{
    uint16_t baudSetting = BAUDRATE_CONFIG_VALUE(F_CPU,baudrate);
    outb(UBRR0H, baudSetting >> 8);
    outb(UBRR0L, baudSetting);
    sbi(UCSR0B, RXEN0);
    sbi(UCSR0B, TXEN0);
    sbi(UCSR0B, RXCIE0);
    sbi(UCSR0B, UDRIE0);
    //sbi(UCSR0B, TXCIE0);
    return 1;
}

void BluetoothUART::end()
{
    flush();
    cbi(UCSR0B, RXEN0);
    cbi(UCSR0B, TXEN0);
    cbi(UCSR0B, RXCIE0);
    cbi(UCSR0B, UDRIE0);
    //cbi(UCSR0B, TXCIE0);
}

int BluetoothUART::available()
{
    return ((int)(BLUETOOTH_BUF_SIZE + m_rx_head - m_rx_tail)) % BLUETOOTH_BUF_SIZE;
}

int BluetoothUART::peek()
{
    if (m_rx_head == m_rx_tail) {
        return -1;
    } else {
        return m_rx_buff[m_rx_tail];
    }
}

int BluetoothUART::read()
{
    if (m_rx_head == m_rx_tail) { // rx buffer is empty
        return -1;
    } else {
        unsigned char c = m_rx_buff[m_rx_tail];
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            m_rx_tail = (m_rx_tail + 1) % BLUETOOTH_BUF_SIZE;
        }        
        return c;
    }
}

size_t BluetoothUART::write(uint8_t data)
{
    uint8_t i = (m_tx_head + 1) % BLUETOOTH_BUF_SIZE;
    while (i == m_tx_tail) { // tx buffer full
        sbi(UCSR0B, UDRIE0);
    }
    m_tx_buff[m_tx_head] = data;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m_tx_head = (m_tx_head + 1) % BLUETOOTH_BUF_SIZE;
        sbi(UCSR0B, UDRIE0);
    }
    return 1;
}

size_t BluetoothUART::write(const uint8_t* ptr, size_t length)
{
    size_t n = 0;
    while (length--) {
        if (write(*ptr++)) n++;
        else break;
    }
    return n;
}

size_t BluetoothUART::write(const char* str)
{
    if (str == NULL) return 0;
    return write((const uint8_t*)str, strlen(str));
}

void BluetoothUART::flush()
{
    while (bit_is_set(UCSR1B, UDRIE1) || bit_is_clear(UCSR1A, TXC1)) {
        if (bit_is_clear(SREG, SREG_I) && bit_is_set(UCSR1B, UDRIE1)) {
            if (bit_is_set(UCSR1A, UDRE1)) {
                txIsr();
            }
        }
    }
}

void BluetoothUART::rxIsr()
{
    unsigned char c = inb(UDR0);
    uint8_t i = (m_rx_head + 1) % BLUETOOTH_BUF_SIZE;
    if (i != m_rx_tail) {
        m_rx_buff[m_rx_head] = c;
        m_rx_head = i;
    }
}

void BluetoothUART::txIsr()
{
    if (m_tx_tail == m_tx_head) { // buffer is empty
        cbi(UCSR0B, UDRIE0); // disable interrupt
    }
    else {
        unsigned char c = m_tx_buff[m_tx_tail];
        m_tx_tail = (m_tx_tail + 1) % BLUETOOTH_BUF_SIZE;
        outb(UDR0, c);
    }
}

BluetoothUART x_bluetooth;

/* EOF */