/*
 * LocalDevice.h
 *
 * Created: 2019-03-27 14:54:33
 *  Author: kong
 */


#ifndef LOCALDEVICE_H_
#define LOCALDEVICE_H_

#include <stdint.h>
#include "../profile/BLEDevice.h"

class BLEService;

class LocalDevice : public BLEDevice
{
public:
    LocalDevice();
    virtual ~LocalDevice();
    int  begin();
    void end();
    virtual bool connected() const;
    virtual bool disconnect();
    virtual char* address() const; //---- String ----
    virtual int rssi();
    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setAdvertisedService(const BLEService& service);
    void setManufacturerData(const uint8_t manufacturerData[], int manufacturerDataLength);
    void setLocalName(const char *localName);
    void setDeviceName(const char* deviceName);
    void setAppearance(uint16_t appearance);
    void addService(BLEService& service);
    int  advertise();
    void stopAdvertise();
    void setEventHandler(BLEDeviceEvent event, BLEDeviceEventHandler eventHandler);
    void setAdvertisingInterval(uint16_t advertisingInterval);
    void setConnectionInterval(uint16_t minimumConnectionInterval, uint16_t maximumConnectionInterval);
    void setConnectable(bool connectable);
    virtual operator bool() const;
    virtual bool operator==(const BLEDevice& rhs) const;
    virtual bool operator!=(const BLEDevice& rhs) const;
    BLEDevice central();

private:
};

extern LocalDevice g_bleDevice;


#endif /* LOCALDEVICE_H_ */