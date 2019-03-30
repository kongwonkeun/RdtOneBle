/*
 * LocalService.cpp
 *
 * Created: 2019-03-29 17:04:48
 *  Author: kong
 */ 


#include "../peripheral/LocalService.h"

LocalService::LocalService(const char* uuid) : BLEAttribute(uuid), m_startHandle(0x0000), m_endHandle(0x0000)
{
    //
}

LocalService::~LocalService()
{
    for (unsigned int i = 0; i < characteristicCount(); i++) {
        LocalCharacteristic* c = characteristic(i);
        if (c->release() <= 0) {
            delete c;
        }
    }
    m_characteristics.clear();
}

enum BLEAttributeType LocalService::type()
{
    return BATT_SERVICE;
}

void LocalService::addCharacteristic(BLECharacteristic& characteristic)
{
    LocalCharacteristic* localCharacteristic = characteristic.local();
    if (localCharacteristic) {
        addCharacteristic(localCharacteristic);
    }
}

void LocalService::setHandles(uint16_t start, uint16_t end)
{
    m_startHandle = start;
    m_endHandle = end;
}

uint16_t LocalService::startHandle()
{
    return m_startHandle;
}

uint16_t LocalService::endHandle()
{
    return m_endHandle;
}

unsigned int LocalService::characteristicCount()
{
    return m_characteristics.size();
}

LocalCharacteristic* LocalService::characteristic(unsigned int index)
{
    return m_characteristics.get(index);
}

void LocalService::addCharacteristic(LocalCharacteristic* characteristic)
{
    characteristic->retain();
    m_characteristics.add(characteristic);
}

