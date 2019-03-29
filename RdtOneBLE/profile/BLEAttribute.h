/*
 * BLEAttribute.h
 *
 * Created: 2019-03-27 11:47:44
 *  Author: kong
 */


#ifndef BLEATTRIBUTE_H_
#define BLEATTRIBUTE_H_

#include <stdint.h>
#include "../profile/BLEUUID.h"

enum BLEAttributeType
{
    BATT_UNKNOWN = 0x0000,
    BATT_SERVICE = 0x2800,
    BATT_CHRACTERISTIC = 0x2803,
    BATT_DESCRIPTOR = 0x2900
};

class BLEAttribute
{
public:
    BLEAttribute(const char* uuid);
    virtual ~BLEAttribute();
    virtual enum BLEAttributeType type() const;
    const char* uuid() const;
    const uint8_t* uuidData() const;
    uint8_t uuidLength() const;
    int  retain();
    int  release();

private:
    BLEUUID m_uuid;
    int m_refCount;
};


#endif /* BLEATTRIBUTE_H_ */