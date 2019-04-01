/*
 * BLEDevice.h
 *
 * Created: 2019-03-27 10:36:39
 *  Author: kong
 */

#ifndef BLEDEVICE_H_
#define BLEDEVICE_H_

#include <stdint.h>

enum BLEDeviceEvent
{
    BD_CONNECTED = 0,
    BD_DISCONNECTED = 1,
    BD_EVENT_LAST
};

class BLEDevice
{
public:
    BLEDevice();
    virtual ~BLEDevice();
    virtual void poll();
    virtual void poll(unsigned long timeout);
    virtual bool connected();
    virtual bool disconnect();
    virtual char* address(); //---- String ----
    virtual int  rssi();
    virtual operator bool();
    virtual bool operator==(BLEDevice& rhs);
    virtual bool operator!=(BLEDevice& rhs);
    char m_addrString[18];
protected:
    friend class ATTClass;
    BLEDevice(uint16_t handle, uint8_t address[6]);
private:
    uint16_t m_handle;
    uint8_t  m_address[6];
};

typedef void (*BLEDeviceEventHandler)(BLEDevice device);

#endif /* BLEDEVICE_H_ */