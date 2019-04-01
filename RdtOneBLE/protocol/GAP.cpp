/*
 * GAP.cpp
 *
 * Created: 2019-03-29 10:02:13
 *  Author: kong
 */

#include <string.h>
#include "profile/BLEUUID.h"
#include "protocol/HCI.h"
#include "protocol/GAP.h"

GAPClass::GAPClass() :
m_advertising(false), m_advertisedServiceUuid(NULL),
m_manufacturerData(NULL), m_manufacturerDataLength(0), m_localName(NULL), m_advertisingInterval(160),
m_connectable(true), m_serviceData(NULL), m_serviceDataLength(0)
{
    //
}

GAPClass::~GAPClass()
{
    //
}

void GAPClass::setAdvertisedServiceUuid(const char* advertisedServiceUuid)
{
    m_advertisedServiceUuid = advertisedServiceUuid;
}

void GAPClass::setManufacturerData(const uint8_t manufacturerData[], int manufacturerDataLength)
{
    m_manufacturerData = manufacturerData;
    m_manufacturerDataLength = manufacturerDataLength;
}

void GAPClass::setLocalName(const char *localName)
{
    m_localName = localName;
}

bool GAPClass::advertising()
{
    return m_advertising;
}

int GAPClass::advertise()
{
    uint8_t directBDAddr[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t type = (m_connectable) ? 0x00 : (m_localName ? 0x02 : 0x03);
    m_advertising = false;
    if (g_hci.setLeAdvertisingParams(m_advertisingInterval, m_advertisingInterval, type, 0x00, 0x00, directBDAddr, 0x07, 0) != 0) {
        return 0;
    }
    uint8_t advertisingData[31];
    uint8_t advertisingDataLen = 0;
    advertisingData[0] = 0x02;
    advertisingData[1] = 0x01;
    advertisingData[2] = 0x06;
    advertisingDataLen += 3;

    if (m_advertisedServiceUuid) {
        BLEUUID serviceUuid(m_advertisedServiceUuid);
        int uuidLen = serviceUuid.length();
        advertisingData[3] = 1 + uuidLen;
        advertisingData[4] = (uuidLen > 2) ? 0x06 : 0x02;
        memcpy(&advertisingData[5], serviceUuid.data(), uuidLen);
        advertisingDataLen += (2 + uuidLen);
    }
    else if (m_manufacturerData && m_manufacturerDataLength) {
        advertisingData[3] = 1 + m_manufacturerDataLength;
        advertisingData[4] = 0xff;
        memcpy(&advertisingData[5], m_manufacturerData, m_manufacturerDataLength);
        advertisingDataLen += (2 + m_manufacturerDataLength);
    }
    if (m_serviceData && m_serviceDataLength > 0 && advertisingDataLen >= (m_serviceDataLength + 4)) {
        advertisingData[advertisingDataLen++] = m_serviceDataLength + 3;
        advertisingData[advertisingDataLen++] = 0x16;
        memcpy(&advertisingData[advertisingDataLen], &m_serviceDataUuid, sizeof(m_serviceDataUuid));
        advertisingDataLen += sizeof(m_serviceDataUuid);
        memcpy(&advertisingData[advertisingDataLen],m_serviceData, m_serviceDataLength);
        advertisingDataLen += m_serviceDataLength;
    }
    if (g_hci.setLeAdvertisingData(advertisingDataLen, advertisingData) != 0) {
        return 0;
    }
    uint8_t scanResponseData[31];
    uint8_t scanResponseDataLen = 0;

    if (m_localName) {
        int localNameLen = strlen(m_localName);
        if (localNameLen > 29) {
            localNameLen = 29;
            scanResponseData[1] = 0x08;
        } else {
            scanResponseData[1] = 0x09;
        }
        scanResponseData[0] = 1 + localNameLen;
        memcpy(&scanResponseData[2], m_localName, localNameLen);
        scanResponseDataLen += (2 + localNameLen);
    }
    if (g_hci.setLeScanResponseData(scanResponseDataLen, scanResponseData) != 0) {
        return 0;
    }
    if (g_hci.setLeAdvertiseEnable(0x01) != 0) {
        return 0;
    }
    m_advertising = false;
    return 1;
}

void GAPClass::stopAdvertise()
{
    m_advertising = false;
    g_hci.setLeAdvertiseEnable(0x00);
}

void GAPClass::setAdvertisingInterval(uint16_t advertisingInterval)
{
    m_advertisingInterval = advertisingInterval;
}

void GAPClass::setConnectable(bool connectable)
{
    m_connectable = connectable;
}

void GAPClass::setAdvertisedServiceData(uint16_t uuid, const uint8_t data[], int length)
{
    m_serviceDataUuid = uuid;
    m_serviceData = data;
    m_serviceDataLength = length;
}

GAPClass g_gap;

/* EOF */