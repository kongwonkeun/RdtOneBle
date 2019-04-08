/*
 * BluetoothUART.h
 *
 * Created: 2019-04-06 11:01:18
 *  Author: kong
 */ 

#ifndef BLUETOOTHUART_H_
#define BLUETOOTHUART_H_

#include <stdint.h>
#include <stdlib.h>

#define BLUETOOTH_BUF_SIZE 64

class BluetoothUART
{
public:
    BluetoothUART();
    ~BluetoothUART();
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
    unsigned char m_rx_buff[BLUETOOTH_BUF_SIZE];
    uint8_t m_tx_head;
    uint8_t m_tx_tail;
    unsigned char m_tx_buff[BLUETOOTH_BUF_SIZE];
};

extern BluetoothUART x_bluetooth;

#endif /* BLUETOOTHUART_H_ */