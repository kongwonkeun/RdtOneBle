/*
 * BLEDescriptor.cpp
 *
 * Created: 2019-03-29 17:21:00
 *  Author: kong
 */

#include <stdlib.h>
#include "peripheral/LocalDescriptor.h"
#include "profile/BLEDescriptor.h"

BLEDescriptor::BLEDescriptor() :
BLEDescriptor(NULL)
{
    //
}

BLEDescriptor::BLEDescriptor(LocalDescriptor* local) : m_local(local)
{
    if (m_local) {
        m_local->retain();
    }
}

BLEDescriptor::BLEDescriptor(const char* uuid, const uint8_t value[], int valueSize) :
BLEDescriptor(new LocalDescriptor(uuid, value, valueSize))
{
    //
}

BLEDescriptor::BLEDescriptor(const char* uuid, const char* value) :
BLEDescriptor(new LocalDescriptor(uuid, value))
{
    //
}

BLEDescriptor::~BLEDescriptor()
{
    if (m_local && m_local->release() <= 0) {
        delete m_local;
    }
}

int BLEDescriptor::valueSize()
{
    if (m_local) {
        return m_local->valueSize();
    }
    return 0;
}

const uint8_t* BLEDescriptor::value()
{
    if (m_local) {
        return m_local->value();
    }
    return NULL;
}

uint8_t BLEDescriptor::operator[] (int offset)
{
    if (m_local) {
        return (*m_local)[offset];
    }
    return 0;
}

BLEDescriptor::operator bool()
{
    return (m_local != NULL);
}

LocalDescriptor* BLEDescriptor::local()
{
    return m_local;
}

/* EOF */