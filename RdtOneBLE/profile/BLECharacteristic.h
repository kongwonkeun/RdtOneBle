/*
 * BLECharacteristic.h
 *
 * Created: 2019-03-27 13:14:55
 *  Author: kong
 */


#ifndef BLECHARACTERISTIC_H_
#define BLECHARACTERISTIC_H_

#include <stdint.h>

class LocalCharacteristic;
class BLEDescriptor;
class BLECharacteristic;
class BLEDevice;

typedef void (*BLECharacteristicEventHandler)(BLEDevice device, BLECharacteristic characteristic);

enum BLECharacteristicEvent
{
    BC_SUBSCRIBED = 0,
    BC_UNSUBSCRIBED = 1,
    BC_READ = 2,
    BC_WRITTEN = 3,
    BC_EVENT_LAST
};

class BLECharacteristic
{
public:
    BLECharacteristic();
    BLECharacteristic(const char* uuid, uint8_t properties, int valueSize, bool fixedLength = false);
    BLECharacteristic(const char* uuid, uint8_t properties, const char* value);
    virtual ~BLECharacteristic();
    uint8_t properties() const;
    int  valueSize() const;
    const uint8_t* value() const;
    int  valueLength() const;
    uint8_t operator[] (int offset) const;
    int  writeValue(const uint8_t value[], int length);
    int  writeValue(const char* value);
    int  setValue(const uint8_t value[], int length) { return writeValue(value, length); }
    int  setValue(const char* value) { return writeValue(value); }
    int  broadcast();
    bool written();
    bool subscribed();
    void addDescriptor(BLEDescriptor& descriptor);
    operator bool() const;
    void setEventHandler(int event, BLECharacteristicEventHandler eventHandler);

protected:
    friend class LocalCharacteristic;
    friend class LocalService;
    BLECharacteristic(LocalCharacteristic* local);
    LocalCharacteristic* local();

private:
    LocalCharacteristic* m_local;
};


#endif /* BLECHARACTERISTIC_H_ */