/*
 * BLEUUID.cpp
 *
 * Created: 2019-03-29 18:01:59
 *  Author: kong
 */

#include <stdlib.h>
#include <string.h>
#include "BLEUUID.h"

BLEUUID::BLEUUID(const char * str) : m_str(str)
{
    char temp[] = {0, 0, 0};
    memset(m_data, 0x00, sizeof(m_data));
    m_length = 0;
    for (int i = strlen(str) - 1; i >= 0 && m_length < BLE_UUID_MAX_LENGTH; i -= 2) {
        if (str[i] == '-') {
            i++;
            continue;
        }
        temp[0] = str[i - 1];
        temp[1] = str[i];
        m_data[m_length] = strtoul(temp, NULL, 16);
        m_length++;
    }
    if (m_length <= 2) {
        m_length = 2;
    } else {
        m_length = 16;
    }
}

const char* BLEUUID::str()
{
    return m_str;
}

const uint8_t* BLEUUID::data()
{
    return m_data;
}

uint8_t BLEUUID::length()
{
    return m_length;
}

/* EOF */