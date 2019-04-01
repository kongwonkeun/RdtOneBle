/*
 * L2CAPSignaling.h
 *
 * Created: 2019-03-26 17:11:21
 *  Author: kong
 */

#ifndef L2CAPSIGNALING_H_
#define L2CAPSIGNALING_H_

#include <stdint.h>

class L2CAPSignalingClass
{
public:
    L2CAPSignalingClass();
    virtual ~L2CAPSignalingClass();
    void addConnection(
         uint16_t handle, uint8_t role, uint8_t peerBDAddrType, uint8_t peerBDAddr[6],
         uint16_t interval, uint16_t latency, uint16_t supervisionTimeout, uint8_t masterClockAccuracy
    );
    void handleData(uint16_t handle, uint8_t length, uint8_t data[]);
    void removeConnection(uint16_t handle, uint16_t reason);
    void setConnectionInterval(uint16_t min, uint16_t max);
private:
    void connParamUpdateReq(uint16_t handle, uint8_t id, uint8_t length, uint8_t data[]);
    void connParamUpdateRes(uint16_t handle, uint8_t id, uint8_t length, uint8_t data[]);
    uint16_t m_minInterval;
    uint16_t m_maxInterval;
};

extern L2CAPSignalingClass g_l2capSignaling;

#define SIGNALING_CID 0x0005
#define CONNECTION_PARAMETER_UPDATE_REQUEST  0x12
#define CONNECTION_PARAMETER_UPDATE_RESPONSE 0x13

#endif /* L2CAPSIGNALING_H_ */