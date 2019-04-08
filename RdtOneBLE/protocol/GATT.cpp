/*
 * GATT.cpp
 *
 * Created: 2019-03-29 10:36:38
 *  Author: kong
 */

#include <stdlib.h>
#include "profile/BLEProperty.h"
#include "profile/BLEService.h"
#include "profile/BLEAttribute.h"
#include "peripheral/LocalDescriptor.h"
#include "protocol/GATT.h"

GATTClass::GATTClass() :
m_genericAccessService("1800"), m_deviceNameCharacteristic("2a00", BPP_READ, 20), m_appearanceCharacteristic("2a01", BPP_READ, 2),
m_genericAttributeService("1801"), m_servicesChangedCharacteristic("2a05", BPP_INDICATE, 4)
{
    m_genericAccessService.retain();
    m_genericAttributeService.retain();
    m_genericAccessService.addCharacteristic(&m_deviceNameCharacteristic);
    m_genericAccessService.addCharacteristic(&m_appearanceCharacteristic);
    m_genericAttributeService.addCharacteristic(&m_servicesChangedCharacteristic);
}

GATTClass::~GATTClass()
{
    clearAttributes();
}

void GATTClass::begin()
{
    setDeviceName("Arduino");
    setAppearance(0x000);
    clearAttributes();
    addService(&m_genericAccessService);
    addService(&m_genericAttributeService);
}

void GATTClass::end()
{
    m_attributes.clear();
}

void GATTClass::setDeviceName(const char* deviceName)
{
    m_deviceNameCharacteristic.writeValue(deviceName);
}

void GATTClass::setAppearance(uint16_t appearance)
{
    m_appearanceCharacteristic.writeValue((uint8_t*)&appearance, sizeof(appearance));
}

void GATTClass::addService(BLEService& service)
{
    LocalService* localService = service.local();
    if (localService) {
        addService(localService);
    }
}

unsigned int GATTClass::attributeCount()
{
    return m_attributes.size();
}

BLEAttribute* GATTClass::attribute(unsigned int index)
{
    return m_attributes.get(index);
}

uint16_t GATTClass::serviceUuidForCharacteristic(LocalCharacteristic* characteristic)
{
    uint16_t serviceUuid = 0x0000;
    LocalService* lastService = NULL;

    for (unsigned int i = 0; i < attributeCount(); i++) {
        BLEAttribute* a = attribute(i);
        uint16_t attributeType = a->type();
        if (attributeType == BATT_SERVICE) {
            lastService = (LocalService*)a;
        } else if (a == characteristic) {
            break;
        }
    }
    if (lastService) {
        if (lastService->uuidLength() == 2) {
            serviceUuid = *(uint16_t*)(lastService->uuidData());
        } else {
            serviceUuid = *(uint16_t*)(lastService->uuidData() + 10);
        }
    }
    return serviceUuid;
}

void GATTClass::addService(LocalService* service)
{
    service->retain();
    m_attributes.add(service);
    uint16_t startHandle = attributeCount();

    for (unsigned int i = 0; i < service->characteristicCount(); i++) {
        LocalCharacteristic* characteristic = service->characteristic(i);
        characteristic->retain();
        m_attributes.add(characteristic);
        characteristic->setHandle(attributeCount());
        // add the characteristic again to make space of the characteristic value handle
        m_attributes.add(characteristic);
        for (unsigned int j = 0; j < characteristic->descriptorCount(); j++) {
            LocalDescriptor* descriptor = characteristic->descriptor(j);
            descriptor->retain();
            m_attributes.add(descriptor);
            descriptor->setHandle(attributeCount());
        }
    }
    service->setHandles(startHandle, attributeCount());
}

void GATTClass::clearAttributes()
{
    for (unsigned int i = 0; i < attributeCount(); i++) {
        BLEAttribute* a = attribute(i);
        if (a->release() <= 0) {
            delete a;
        }
    }
    m_attributes.clear();
}

GATTClass x_gatt;

/* EOF */