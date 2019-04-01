/*
 * GAP.h
 *
 * Created: 2019-03-27 11:22:42
 *  Author: kong
 */

#ifndef GAP_H_
#define GAP_H_

#include <stdint.h>

class GAPClass
{
public:
    GAPClass();
    virtual ~GAPClass();
    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setManufacturerData(const uint8_t manufacturerData[], int manufacturerDataLength);
    void setLocalName(const char *localName);
    bool advertising();
    int  advertise();
    void stopAdvertise();
    void setAdvertisingInterval(uint16_t advertisingInterval);
    void setConnectable(bool connectable);
protected:
    friend class LocalCharacteristic;
    void setAdvertisedServiceData(uint16_t uuid, const uint8_t data[], int length);
private:
    bool m_advertising;
    const char* m_advertisedServiceUuid;
    const uint8_t* m_manufacturerData;
    int  m_manufacturerDataLength;
    const char* m_localName;
    uint16_t m_advertisingInterval;
    bool m_connectable;
    const uint8_t* m_serviceData;
    int  m_serviceDataLength;
    uint16_t m_serviceDataUuid;
};

extern GAPClass g_gap;

#endif /* GAP_H_ */