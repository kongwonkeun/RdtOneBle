/*
 * LocalDescriptor.cpp
 *
 * Created: 2019-03-29 16:06:35
 *  Author: kong
 */

#include <string.h>
#include "system/XUtil.h"
#include "peripheral/LocalDescriptor.h"

LocalDescriptor::LocalDescriptor(const char* uuid, const uint8_t value[], int valueSize) : BLEAttribute(uuid),
m_value(value), m_valueSize(min(valueSize, 512)), m_handle(0x0000)
{
    //
}

LocalDescriptor::LocalDescriptor(const char* uuid, const char* value) : LocalDescriptor(uuid, (const uint8_t*)value, strlen(value))
{
    //
}

LocalDescriptor::~LocalDescriptor()
{
    //
}

enum BLEAttributeType LocalDescriptor::type()
{
    return BATT_DESCRIPTOR;
}

int LocalDescriptor::valueSize()
{
    return m_valueSize;
}

const uint8_t* LocalDescriptor::value()
{
    return m_value;
}

uint8_t LocalDescriptor::operator[] (int offset)
{
    return m_value[offset];
}

void LocalDescriptor::setHandle(uint16_t handle)
{
    m_handle = handle;
}

uint16_t LocalDescriptor::handle()
{
    return m_handle;
}

/* EOF */