/*
 * BLEAttribute.cpp
 *
 * Created: 2019-03-29 17:11:01
 *  Author: kong
 */ 


#include "../profile/BLEAttribute.h"

BLEAttribute::BLEAttribute(const char* uuid) : m_uuid(uuid), m_refCount(0)
{
    //
}

BLEAttribute::~BLEAttribute()
{
    //
}

const char* BLEAttribute::uuid()
{
    return m_uuid.str();
}

const uint8_t* BLEAttribute::uuidData()
{
    return m_uuid.data();
}

uint8_t BLEAttribute::uuidLength()
{
    return m_uuid.length();
}

enum BLEAttributeType BLEAttribute::type()
{
    return BATT_UNKNOWN;
}

int BLEAttribute::retain()
{
    m_refCount++;
    return m_refCount;
}

int BLEAttribute::release()
{
    m_refCount--;
    return m_refCount;
}

