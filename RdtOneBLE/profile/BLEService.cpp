/*
 * BLEService.cpp
 *
 * Created: 2019-03-29 17:32:50
 *  Author: kong
 */

#include <stdlib.h>
#include "peripheral/LocalService.h"
#include "profile/BLECharacteristic.h"
#include "profile/BLEService.h"

BLEService::BLEService() : BLEService((LocalService*)NULL)
{
    //
}

BLEService::BLEService(LocalService* local) : m_local(local)
{
    if (m_local) {
        m_local->retain();
    }
}

BLEService::BLEService(const char* uuid) : BLEService(new LocalService(uuid))
{
    //
}

BLEService::~BLEService()
{
    if (m_local && m_local->release() <= 0) {
        delete m_local;
    }
}

const char* BLEService::uuid()
{
    if (m_local) {
        return m_local->uuid();
    }
    return "";
}

void BLEService::addCharacteristic(BLECharacteristic& characteristic)
{
    if (m_local) {
        m_local->addCharacteristic(characteristic);
    }
}

BLEService::operator bool()
{
    return (m_local != NULL);
}

LocalService* BLEService::local()
{
    return m_local;
}

/* EOF */