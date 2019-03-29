/*
 * BLEDescriptor.h
 *
 * Created: 2019-03-27 13:27:16
 *  Author: kong
 */


#ifndef BLEDESCRIPTOR_H_
#define BLEDESCRIPTOR_H_

#include <stdint.h>

class LocalDescriptor;

class BLEDescriptor
{
public:
    BLEDescriptor();
    BLEDescriptor(const char* uuid, const uint8_t value[], int valueSize);
    BLEDescriptor(const char* uuid, const char* value);
    virtual ~BLEDescriptor();
    int valueSize() const;
    const uint8_t* value() const;
    uint8_t operator[] (int offset) const;
    operator bool() const;

protected:
    friend class LocalCharacteristic;
    BLEDescriptor(LocalDescriptor* local);
    LocalDescriptor* local();

private:
    LocalDescriptor* m_local;
};


#endif /* BLEDESCRIPTOR_H_ */