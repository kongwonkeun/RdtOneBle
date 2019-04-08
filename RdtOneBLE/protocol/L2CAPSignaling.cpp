/*
 * L2CAPSignaling.cpp
 *
 * Created: 2019-03-29 14:47:53
 *  Author: kong
 */

#include "protocol/HCI.h"
#include "protocol/L2CAPSignaling.h"

L2CAPSignalingClass::L2CAPSignalingClass() : m_minInterval(0), m_maxInterval(0)
{
    // just initialize properties
}

L2CAPSignalingClass::~L2CAPSignalingClass()
{
    // nothing to do
}

void L2CAPSignalingClass::addConnection(uint16_t handle, uint8_t role, uint8_t, uint8_t[6], uint16_t interval, uint16_t, uint16_t, uint8_t)
{
    if (role != 1) {
        return;
    }
    if (!m_minInterval || !m_maxInterval) { // no connection intervals to request
        return;
    }
    if (interval >= m_minInterval && interval <= m_maxInterval) { // all good, within interval range
        return;
    }

    struct __attribute__ ((packed)) L2CAPConnectionParameterUpdateRequest {
        uint8_t  code;
        uint8_t  identifier;
        uint16_t length;
        uint16_t minInterval;
        uint16_t maxInterval;
        uint16_t latency;
        uint16_t supervisionTimeout;
    } request = { CONNECTION_PARAMETER_UPDATE_REQUEST, 0x01, 8, m_minInterval, m_maxInterval, 0x0000, 0x00c8 };

    x_hci.sendAclPacket(handle, SIGNALING_CID, sizeof(request), &request);
}

void L2CAPSignalingClass::handleData(uint16_t handle, uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) L2CAPSignalingHdr {
        uint8_t  code;
        uint8_t  identifier;
        uint16_t length;
    } *l2capSignalingHdr = (L2CAPSignalingHdr*)data;

    if (length < sizeof(L2CAPSignalingHdr)) { // too short, ignore
        return;
    }
    if (length != (sizeof(L2CAPSignalingHdr) + l2capSignalingHdr->length)) { // invalid length, ignore
        return;
    }
    uint8_t  code = l2capSignalingHdr->code;
    uint8_t  identifier = l2capSignalingHdr->identifier;
    uint16_t len = l2capSignalingHdr->length;
    data = &data[sizeof(L2CAPSignalingHdr)];

    if (code == CONNECTION_PARAMETER_UPDATE_REQUEST) {
        connParamUpdateReq(handle, identifier, len, data);
    } else if (code == CONNECTION_PARAMETER_UPDATE_RESPONSE) {
        connParamUpdateRes(handle, identifier, len, data);
    }
}

void L2CAPSignalingClass::removeConnection(uint16_t, uint16_t)
{
    //
}

void L2CAPSignalingClass::setConnectionInterval(uint16_t min, uint16_t max)
{
    m_minInterval = min;
    m_maxInterval = max;
}

void L2CAPSignalingClass::connParamUpdateReq(uint16_t handle, uint8_t identifier, uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) L2CAPConnectionParameterUpdateRequest {
        uint16_t minInterval;
        uint16_t maxInterval;
        uint16_t latency;
        uint16_t supervisionTimeout;
    } *request = (L2CAPConnectionParameterUpdateRequest*)data;

    if (length < sizeof(L2CAPConnectionParameterUpdateRequest)) { // too short, ignore
        return;
    }
    struct __attribute__ ((packed)) L2CAPConnectionParameterUpdateResponse {
        uint8_t  code;
        uint8_t  identifier;
        uint16_t length;
        uint16_t value;
    } response = { CONNECTION_PARAMETER_UPDATE_RESPONSE, identifier, 2, 0x0000 };

    if (m_minInterval && m_maxInterval) {
        if (request->minInterval < m_minInterval || request->maxInterval > m_maxInterval) {
            response.value = 0x0001; // reject
        }
    }
    x_hci.sendAclPacket(handle, SIGNALING_CID, sizeof(response), &response);
    if (response.value == 0x0000) {
        x_hci.updateLeConnection(handle, request->minInterval, request->maxInterval, request->latency, request->supervisionTimeout);
    }
}

void L2CAPSignalingClass::connParamUpdateRes(uint16_t, uint8_t, uint8_t, uint8_t[])
{
    //
}

L2CAPSignalingClass x_l2capSignaling;

/* EOF */