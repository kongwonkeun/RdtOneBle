/*
 * LocalDescriptor.h
 *
 * Created: 2019-03-27 14:51:08
 *  Author: kong
 */


#ifndef LOCALDESCRIPTOR_H_
#define LOCALDESCRIPTOR_H_

#include <stdint.h>
#include "../profile/BLEAttribute.h"

class LocalDescriptor : public BLEAttribute
{
public:
    LocalDescriptor(const char* uuid, const uint8_t value[], int valueSize);
    LocalDescriptor(const char* uuid, const char* value);
    virtual ~LocalDescriptor();
    virtual enum BLEAttributeType type();
    int valueSize();
    const uint8_t* value();
    uint8_t operator[] (int offset);

protected:
    friend class GATTClass;
    void setHandle(uint16_t handle);
    uint16_t handle();

private:
    const uint8_t* m_value;
    int m_valueSize;
    uint16_t m_handle;
};


#endif /* LOCALDESCRIPTOR_H_ */