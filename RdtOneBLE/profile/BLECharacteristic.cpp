/*
 * BLECharacteristic.cpp
 *
 * Created: 2019-03-29 17:14:47
 *  Author: kong
 */ 


#include "../profile/BLECharacteristic.h"
#include "../peripheral/LocalCharacteristic.h"


BLECharacteristic::BLECharacteristic() : BLECharacteristic(NULL)
{
    //
}

BLECharacteristic::BLECharacteristic(LocalCharacteristic* local) : m_local(local)
{
    if (m_local) {
        m_local->retain();
    }
}

BLECharacteristic::BLECharacteristic(const char* uuid, uint8_t properties, int valueSize, bool fixedLength) :
BLECharacteristic(new LocalCharacteristic(uuid, properties, valueSize, fixedLength))
{
    //
}

BLECharacteristic::BLECharacteristic(const char* uuid, uint8_t properties, const char* value) :
BLECharacteristic(new LocalCharacteristic(uuid, properties, value))
{
    //
}

BLECharacteristic::~BLECharacteristic()
{
    if (m_local && m_local->release() <= 0) {
        delete m_local;
    }
}

uint8_t BLECharacteristic::properties()
{
    if (m_local) {
        return m_local->properties();
    }
    return 0;
}

int BLECharacteristic::valueSize()
{
    if (m_local) {
        return m_local->valueSize();
    }
    return 0;
}

const uint8_t* BLECharacteristic::value()
{
    if (m_local) {
        return m_local->value();
    }
    return NULL;
}

int BLECharacteristic::valueLength()
{
    if (m_local) {
        return m_local->valueLength();
    }
    return 0;
}

uint8_t BLECharacteristic::operator[] (int offset)
{
    if (m_local) {
        return (*m_local)[offset];
    }
    return 0;
}

int BLECharacteristic::writeValue(const uint8_t value[], int length)
{
    if (m_local) {
        return m_local->writeValue(value, length);
    }
    return 0;
}

int BLECharacteristic::writeValue(const char* value)
{
    if (m_local) {
        return m_local->writeValue(value);
    }
    return 0;
}

int BLECharacteristic::broadcast()
{
    if (m_local) {
        return m_local->broadcast();
    }
    return 0;
}

bool BLECharacteristic::written()
{
    if (m_local) {
        return m_local->written();
    }
    return false;
}

bool BLECharacteristic::subscribed()
{
    if (m_local) {
        return m_local->subscribed();
    }
    return false;
}

void BLECharacteristic::addDescriptor(BLEDescriptor& descriptor)
{
    if (m_local) {
        return m_local->addDescriptor(descriptor);
    }
}

BLECharacteristic::operator bool()
{
    return (m_local != NULL);
}

LocalCharacteristic* BLECharacteristic::local()
{
    return m_local;
}

void BLECharacteristic::setEventHandler(int event, BLECharacteristicEventHandler eventHandler)
{
    if (m_local) {
        m_local->setEventHandler((BLECharacteristicEvent)event, eventHandler);
    }
}


