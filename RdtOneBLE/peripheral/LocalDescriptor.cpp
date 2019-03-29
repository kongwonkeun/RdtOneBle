﻿/*
 * LocalDescriptor.cpp
 *
 * Created: 2019-03-29 16:06:35
 *  Author: kong
 */ 


#include "../peripheral/LocalDescriptor.h"
#include "../system/XUtil.h"
#include <string.h>

LocalDescriptor::LocalDescriptor(const char* uuid, const uint8_t value[], int valueSize) : BLEAttribute(uuid),
m_value(value), m_valueSize(min(valueSize, 512)), m_handle(0x0000)
{
    // just initialize properties
}

LocalDescriptor::LocalDescriptor(const char* uuid, const char* value) : LocalDescriptor(uuid, (const uint8_t*)value, strlen(value))
{
    // nothing to do
}

LocalDescriptor::~LocalDescriptor()
{
    // nothing to do
}

enum BLEAttributeType LocalDescriptor::type() const
{
    return BATT_DESCRIPTOR;
}

int LocalDescriptor::valueSize() const
{
    return m_valueSize;
}

const uint8_t* LocalDescriptor::value() const
{
    return m_value;
}

uint8_t LocalDescriptor::operator[] (int offset) const
{
    return m_value[offset];
}

void LocalDescriptor::setHandle(uint16_t handle)
{
    m_handle = handle;
}

uint16_t LocalDescriptor::handle() const
{
    return m_handle;
}

