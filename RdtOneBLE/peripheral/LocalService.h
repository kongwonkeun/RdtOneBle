﻿/*
 * LocalService.h
 *
 * Created: 2019-03-27 15:03:47
 *  Author: kong
 */

#ifndef LOCALSERVICE_H_
#define LOCALSERVICE_H_

#include <stdint.h>
//#include "peripheral/LocalCharacteristic.h"
//#include "profile/BLECharacteristic.h"
#include "profile/BLEAttribute.h"
#include "system/LinkedList.h"

class LocalCharacteristic;
class BLECharacteristic;

class LocalService : public BLEAttribute
{
public:
    LocalService(const char* uuid);
    virtual ~LocalService();
    virtual enum BLEAttributeType type();
    void addCharacteristic(BLECharacteristic& characteristic);
protected:
    friend class ATTClass;
    friend class GATTClass;
    void setHandles(uint16_t start, uint16_t end);
    uint16_t startHandle();
    uint16_t endHandle();
    unsigned int characteristicCount();
    LocalCharacteristic* characteristic(unsigned int index);
    void addCharacteristic(LocalCharacteristic* characteristic);
private:
    uint16_t m_startHandle;
    uint16_t m_endHandle;
    LinkedList<LocalCharacteristic*> m_characteristics;
};

#endif /* LOCALSERVICE_H_ */