/*
 * LocalDevice.cpp
 *
 * Created: 2019-03-29 16:25:37
 *  Author: kong
 */

#include <stdio.h>
#include <util/delay.h>
#include "protocol/HCI.h"
#include "protocol/GATT.h"
#include "protocol/GAP.h"
#include "protocol/ATT.h"
#include "protocol/L2CAPSignaling.h"
#include "profile/BLEService.h"
#include "peripheral/LocalDevice.h"

LocalDevice::LocalDevice()
{
    //
}

LocalDevice::~LocalDevice()
{
    //
}

int LocalDevice::begin()
{
    if (!x_hci.begin()) {
        end();
        return 0;
    }
    _delay_ms(100);
    if (x_hci.reset() != 0) {
        end();
        return 0;
    }
    uint8_t  hciVer;
    uint16_t hciRev;
    uint8_t  lmpVer;
    uint16_t manufacturer;
    uint16_t lmpSubVer;
    if (x_hci.readLocalVersion(hciVer, hciRev, lmpVer, manufacturer, lmpSubVer) != 0) {
        end();
        return 0;
    }
    if (x_hci.setEventMask(0x3FFFFFFFFFFFFFFF) != 0) {
        end();
        return 0;
    }
    uint16_t pktLen;
    uint8_t maxPkt;
    if (x_hci.readLeBufferSize(pktLen, maxPkt) != 0) {
        end();
        return 0;
    }
    x_gatt.begin();
    return 1;
}

void LocalDevice::end()
{
    x_gatt.end();
    x_hci.end();
}

bool LocalDevice::connected()
{
    x_hci.poll();
    return x_att.connected();
}

bool LocalDevice::disconnect()
{
    return x_att.disconnect();
}

char* LocalDevice::address()
{
    uint8_t addr[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    x_hci.readBDAddr(addr);
    sprintf(m_addrString, "%02x:%02x:%02x:%02x:%02x:%02x", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    return  m_addrString;
}

int LocalDevice::rssi()
{
    BLEDevice central = x_att.central();
    if (central) {
        return central.rssi();
    }
    return 127;
}

void LocalDevice::setAdvertisedServiceUuid(const char* advertisedServiceUuid)
{
    x_gap.setAdvertisedServiceUuid(advertisedServiceUuid);
}

void LocalDevice::setAdvertisedService(BLEService& service)
{
    setAdvertisedServiceUuid(service.uuid());
}

void LocalDevice::setManufacturerData(const uint8_t manufacturerData[], int manufacturerDataLength)
{
    x_gap.setManufacturerData(manufacturerData, manufacturerDataLength);
}

void LocalDevice::setLocalName(const char *localName)
{
    x_gap.setLocalName(localName);
}

void LocalDevice::setDeviceName(const char* deviceName)
{
    x_gatt.setDeviceName(deviceName);
}

void LocalDevice::setAppearance(uint16_t appearance)
{
    x_gatt.setAppearance(appearance);
}

void LocalDevice::addService(BLEService& service)
{
    x_gatt.addService(service);
}

int LocalDevice::advertise()
{
    return x_gap.advertise();
}

void LocalDevice::stopAdvertise()
{
    x_gap.stopAdvertise();
}

BLEDevice LocalDevice::central()
{
    x_hci.poll();
    return x_att.central();
}

void LocalDevice::setEventHandler(BLEDeviceEvent event, BLEDeviceEventHandler eventHandler)
{
    x_att.setEventHandler(event, eventHandler);
}

void LocalDevice::setAdvertisingInterval(uint16_t advertisingInterval)
{
    x_gap.setAdvertisingInterval(advertisingInterval);
}

void LocalDevice::setConnectionInterval(uint16_t minimumConnectionInterval, uint16_t maximumConnectionInterval)
{
    x_l2capSignaling.setConnectionInterval(minimumConnectionInterval, maximumConnectionInterval);
}

void LocalDevice::setConnectable(bool connectable)
{
    x_gap.setConnectable(connectable);
}

LocalDevice::operator bool()
{
    return true;
}

bool LocalDevice::operator==(BLEDevice& rhs)
{
    return (this == &rhs);
}

bool LocalDevice::operator!=(BLEDevice& rhs)
{
    return (this != &rhs);
}

LocalDevice g_bleDevice;

/* EOF */