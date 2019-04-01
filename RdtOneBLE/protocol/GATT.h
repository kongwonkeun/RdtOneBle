/*
 * GATT.h
 *
 * Created: 2019-03-27 11:38:28
 *  Author: kong
 */

#ifndef GATT_H_
#define GATT_H_

#include <stdint.h>
#include "system/XLinkedList.h"
#include "peripheral/LocalCharacteristic.h"
#include "peripheral/LocalService.h"

class BLEAttribute;
class BLEService;

class GATTClass
{
public:
    GATTClass();
    virtual ~GATTClass();
    void begin();
    void end();
    void setDeviceName(const char* deviceName);
    void setAppearance(uint16_t appearance);
    void addService(BLEService& service);
protected:
    friend class ATTClass;
    friend class LocalCharacteristic;
    unsigned int  attributeCount();
    BLEAttribute* attribute(unsigned int index);
    uint16_t serviceUuidForCharacteristic(LocalCharacteristic* characteristic);
private:
    void addService(LocalService* service);
    void clearAttributes();
    LocalService        m_genericAccessService;
    LocalCharacteristic m_deviceNameCharacteristic;
    LocalCharacteristic m_appearanceCharacteristic;
    LocalService        m_genericAttributeService;
    LocalCharacteristic m_servicesChangedCharacteristic;
    XLinkedList<BLEAttribute*> m_attributes; //----
};

extern GATTClass g_gatt;

#endif /* GATT_H_ */