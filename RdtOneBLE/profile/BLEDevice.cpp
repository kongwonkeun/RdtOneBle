/*
 * BLEDevice.cpp
 *
 * Created: 2019-03-29 17:26:22
 *  Author: kong
 */ 


#include "../profile/BLEDevice.h"
#include "../protocol/HCI.h"
#include "../protocol/ATT.h"
#include <string.h>
#include <stdio.h>


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
    g_hci.poll();
}

void BLEDevice::poll(unsigned long timeout)
{
    g_hci.poll(timeout);
}

bool BLEDevice::connected() const
{
    g_hci.poll();
    if (!(*this)) {
        return false;
    }
    return g_att.connected(m_handle, m_address);
}

bool BLEDevice::disconnect()
{
    if (m_handle != 0xffff) {
        return g_hci.disconnect(m_handle);
    }
    return false;
}

char* BLEDevice::address() const
{
    char result[18];
    sprintf(result, "%02x:%02x:%02x:%02x:%02x:%02x", m_address[5], m_address[4], m_address[3], m_address[2], m_address[1], m_address[0]);
    return result;
}

int BLEDevice::rssi()
{
    if (m_handle != 0xffff) {
        return g_hci.readRssi(m_handle);
    }
    return 127;
}

BLEDevice::operator bool() const
{
    uint8_t zeros[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
    return ((m_handle != 0xffff) && memcmp(m_address, zeros, sizeof(zeros)) != 0);
}

bool BLEDevice::operator==(const BLEDevice& rhs) const
{
    return ((m_handle == rhs.m_handle) && memcmp(m_address, rhs.m_address, sizeof(m_address)) == 0);
}

bool BLEDevice::operator!=(const BLEDevice& rhs) const
{
    return ((m_handle != rhs.m_handle) || memcmp(m_address, rhs.m_address, sizeof(m_address)) != 0);
}


