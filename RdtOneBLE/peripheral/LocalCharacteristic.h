/*
 * LocalCharacteristic.h
 *
 * Created: 2019-03-27 14:31:41
 *  Author: kong
 */


#ifndef LOCALCHARACTERISTIC_H_
#define LOCALCHARACTERISTIC_H_

#include <stdint.h>
#include "../profile/BLEAttribute.h"
#include "../profile/BLECharacteristic.h"
#include "../profile/BLEDescriptor.h"
#include "../profile/BLEDevice.h"
#include "../peripheral/LocalDescriptor.h"
#include "../system/XLinkedList.h"

class LocalCharacteristic : public BLEAttribute
{
public:
    LocalCharacteristic(const char* uuid, uint8_t properties, int valueSize, bool fixedLength = false);
    LocalCharacteristic(const char* uuid, uint8_t properties, const char* value);
    virtual ~LocalCharacteristic();
    virtual enum BLEAttributeType type();
    uint8_t properties();
    int valueSize();
    const uint8_t* value();
    int  valueLength();
    uint8_t operator[] (int offset);
    int  writeValue(const uint8_t value[], int length);
    int  writeValue(const char* value);
    int  broadcast();
    bool written();
    bool subscribed();
    void addDescriptor(BLEDescriptor& descriptor);
    void setEventHandler(BLECharacteristicEvent event, BLECharacteristicEventHandler eventHandler);

protected:
    friend class ATTClass;
    friend class GATTClass;
    void setHandle(uint16_t handle);
    uint16_t handle();
    uint16_t valueHandle();
    unsigned int descriptorCount();
    LocalDescriptor* descriptor(unsigned int index);
    void readValue(BLEDevice device, uint16_t offset, uint8_t value[], int length);
    void writeValue(BLEDevice device, const uint8_t value[], int length);
    void writeCccdValue(BLEDevice device, uint16_t value);

private:
    uint8_t m_properties;
    int m_valueSize;
    uint8_t* m_value;
    uint8_t  m_valueLength;
    bool m_fixedLength;
    uint16_t m_handle;
    bool m_broadcast;
    bool m_written;
    uint16_t m_cccdValue;
    XLinkedList<LocalDescriptor*> m_descriptors;
    BLECharacteristicEventHandler m_eventHandlers[BC_EVENT_LAST];
};


#endif /* LOCALCHARACTERISTIC_H_ */