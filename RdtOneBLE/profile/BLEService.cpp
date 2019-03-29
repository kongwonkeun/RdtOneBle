/*
 * BLEService.cpp
 *
 * Created: 2019-03-29 17:32:50
 *  Author: kong
 */ 


#include "../profile/BLEService.h"
#include "../profile/BLECharacteristic.h"
#include "../peripheral/LocalService.h"

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

const char* BLEService::uuid() const
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

LocalService* BLEService::local()
{
    return m_local;
}

