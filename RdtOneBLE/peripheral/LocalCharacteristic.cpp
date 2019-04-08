/*
 * LocalCharacteristic.cpp
 *
 * Created: 2019-03-29 15:35:10
 *  Author: kong
 */

#include <string.h>
#include "system/Util.h"
#include "protocol/ATT.h"
#include "protocol/GAP.h"
#include "protocol/GATT.h"
#include "profile/BLEProperty.h"
#include "profile/BLEDescriptor.h"
#include "peripheral/LocalDescriptor.h"
#include "peripheral/LocalCharacteristic.h"

LocalCharacteristic::LocalCharacteristic(const char* uuid, uint8_t properties, int valueSize, bool fixedLength) : BLEAttribute(uuid),
m_properties(properties), m_valueSize(min(valueSize, 512)), m_valueLength(0), m_fixedLength(fixedLength),
m_handle(0x0000), m_broadcast(false), m_written(false), m_cccdValue(0x0000)
{
    memset(m_eventHandlers, 0x00, sizeof(m_eventHandlers));
    if (properties & (BPP_NOTIPY | BPP_INDICATE)) {
        LocalDescriptor* cccd = new LocalDescriptor("2902", (uint8_t*)&m_cccdValue, sizeof(m_cccdValue));
        m_descriptors.add(cccd);
    }
    m_value = (uint8_t*)malloc(valueSize);
}

LocalCharacteristic::LocalCharacteristic(const char* uuid, uint8_t properties, const char* value) : LocalCharacteristic(uuid, properties, strlen(value))
{
    writeValue(value);
}

LocalCharacteristic::~LocalCharacteristic()
{
    for (unsigned int i = 0; i < descriptorCount(); i++) {
        LocalDescriptor* d = descriptor(i);
        if (d->release() <= 0) {
            delete d;
        }
    }
    m_descriptors.clear();
    if (m_value) {
        free(m_value);
    }
}

enum BLEAttributeType LocalCharacteristic::type()
{
    return BATT_CHRACTERISTIC;
}

uint8_t LocalCharacteristic::properties()
{
    return m_properties;
}

int LocalCharacteristic::valueSize()
{
    return m_valueSize;
}

const uint8_t* LocalCharacteristic::value()
{
    return m_value;
}

int LocalCharacteristic::valueLength()
{
    return m_valueLength;
}

uint8_t LocalCharacteristic::operator[] (int offset)
{
    return m_value[offset];
}

int LocalCharacteristic::writeValue(const uint8_t value[], int length)
{
    m_valueLength = min(length, m_valueSize);
    memcpy(m_value, value, m_valueLength);
    if (m_fixedLength) {
        m_valueLength = m_valueSize;
    }
    if ((m_properties & BPP_INDICATE) && (m_cccdValue & 0x0002)) {
        return x_att.handleInd(valueHandle(), m_value, m_valueLength);
    }
    else if ((m_properties & BPP_NOTIPY) && (m_cccdValue & 0x0001)) {
        return x_att.handleNotify(valueHandle(), m_value, m_valueLength);
    }
    if (m_broadcast) {
        uint16_t serviceUuid = x_gatt.serviceUuidForCharacteristic(this);
        x_gap.setAdvertisedServiceData(serviceUuid, value, length);
        if (!x_att.connected() && x_gap.advertising()) {
            x_gap.advertise();
        }
    }
    return 1;
}

int LocalCharacteristic::writeValue(const char* value)
{
    return writeValue((uint8_t*)value, strlen(value));
}

int LocalCharacteristic::broadcast()
{
    if (m_properties & BPP_BROADCAST) {
        m_broadcast = true;
        return 1;
    }
    return 0;
}

bool LocalCharacteristic::written()
{
    bool written = m_written;
    m_written = false;
    return written;
}

bool LocalCharacteristic::subscribed()
{
    return (m_cccdValue != 0x0000);
}

void LocalCharacteristic::addDescriptor(BLEDescriptor& descriptor)
{
    LocalDescriptor* localDescriptor = descriptor.local();
    if (localDescriptor) {
        localDescriptor->retain();
        m_descriptors.add(localDescriptor);
    }
}

void LocalCharacteristic::setEventHandler(BLECharacteristicEvent event, BLECharacteristicEventHandler eventHandler)
{
    if (event < (sizeof(m_eventHandlers) / sizeof(m_eventHandlers[0]))) {
        m_eventHandlers[event] = eventHandler;
    }
}

void LocalCharacteristic::setHandle(uint16_t handle)
{
    m_handle = handle;
}

uint16_t LocalCharacteristic::handle()
{
    return m_handle;
}

uint16_t LocalCharacteristic::valueHandle()
{
    return (m_handle + 1);
}

unsigned int LocalCharacteristic::descriptorCount()
{
    return m_descriptors.size();
}

LocalDescriptor* LocalCharacteristic::descriptor(unsigned int index)
{
    return m_descriptors.get(index);
}

void LocalCharacteristic::readValue(BLEDevice device, uint16_t offset, uint8_t value[], int length)
{
    if (m_eventHandlers[BC_READ]) {
        m_eventHandlers[BC_READ](device, BLECharacteristic(this));
    }
    memcpy(value, m_value + offset, length);
}

void LocalCharacteristic::writeValue(BLEDevice device, const uint8_t value[], int length)
{
    m_written = true;
    writeValue(value, length);
    if (m_eventHandlers[BC_WRITTEN]) {
        m_eventHandlers[BC_WRITTEN](device, BLECharacteristic(this));
    }
}

void LocalCharacteristic::writeCccdValue(BLEDevice device, uint16_t value)
{
    value &= 0x0003;
    if (m_cccdValue != value) {
        m_cccdValue = value;
        BLECharacteristicEvent event = (m_cccdValue) ? BC_SUBSCRIBED : BC_UNSUBSCRIBED;
        if (m_eventHandlers[event]) {
            m_eventHandlers[event](device, BLECharacteristic(this));
        }
    }
}

/* EOF */