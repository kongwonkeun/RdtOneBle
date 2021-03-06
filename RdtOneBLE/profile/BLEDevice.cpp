﻿/*
 * BLEDevice.cpp
 *
 * Created: 2019-03-29 17:26:22
 *  Author: kong
 */

#include <stdio.h>
#include <string.h>
#include "protocol/HCI.h"
#include "protocol/ATT.h"
#include "profile/BLEDevice.h"

BLEDevice::BLEDevice() : m_handle(0xffff)
{
    memset(m_address, 0x00, sizeof(m_address));
}

BLEDevice::BLEDevice(uint16_t handle, uint8_t address[6]) : m_handle(handle)
{
    memcpy(m_address, address, sizeof(m_address));
}

BLEDevice::~BLEDevice()
{
    //
}

void BLEDevice::poll()
{
    x_hci.poll();
}

void BLEDevice::poll(unsigned long timeout)
{
    x_hci.poll(timeout);
}

bool BLEDevice::connected()
{
    x_hci.poll();
    if (!(*this)) {
        return false;
    }
    return x_att.connected(m_handle, m_address);
}

bool BLEDevice::disconnect()
{
    if (m_handle != 0xffff) {
        return x_hci.disconnect(m_handle);
    }
    return false;
}

char* BLEDevice::address()
{
    sprintf(m_addrString, "%02x:%02x:%02x:%02x:%02x:%02x", m_address[5], m_address[4], m_address[3], m_address[2], m_address[1], m_address[0]);
    return  m_addrString;
}

int BLEDevice::rssi()
{
    if (m_handle != 0xffff) {
        return x_hci.readRssi(m_handle);
    }
    return 127;
}

BLEDevice::operator bool()
{
    uint8_t zeros[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
    return ((m_handle != 0xffff) && memcmp(m_address, zeros, sizeof(zeros)) != 0);
}

bool BLEDevice::operator==(BLEDevice& rhs)
{
    return ((m_handle == rhs.m_handle) && memcmp(m_address, rhs.m_address, sizeof(m_address)) == 0);
}

bool BLEDevice::operator!=(BLEDevice& rhs)
{
    return ((m_handle != rhs.m_handle) || memcmp(m_address, rhs.m_address, sizeof(m_address)) != 0);
}

/* EOF */