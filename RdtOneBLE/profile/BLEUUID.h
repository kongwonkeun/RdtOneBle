/*
 * BLEUUID.h
 *
 * Created: 2019-03-27 14:25:43
 *  Author: kong
 */

#ifndef BLEUUID_H_
#define BLEUUID_H_

#include <stdint.h>

#define BLE_UUID_MAX_LENGTH 16

class BLEUUID
{
public:
    BLEUUID(const char* str);
    const char* str();
    const uint8_t* data();
    uint8_t length();
private:
    const char* m_str;
    uint8_t m_data[BLE_UUID_MAX_LENGTH];
    uint8_t m_length;
};

#endif /* BLEUUID_H_ */