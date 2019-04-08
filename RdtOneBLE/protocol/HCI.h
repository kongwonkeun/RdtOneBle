/*
 * HCI.h
 *
 * Created: 2019-03-26 17:37:26
 *  Author: kong
 */

#ifndef HCI_H_
#define HCI_H_

#include <stdint.h>
#include <stdlib.h>

class HCIClass
{
public:
    HCIClass();
    virtual ~HCIClass();
    int  begin();
    void end();
    void poll();
    void poll(unsigned long timeout);
    int  reset();
    int  readLocalVersion(
         uint8_t& hciVer, uint16_t& hciRev, uint8_t& lmpVer,
         uint16_t& manufacturer, uint16_t& lmpSubVer
    );
    int  readBDAddr(uint8_t addr[6]);
    int  readRssi(uint16_t handle);
    int  setEventMask(uint64_t eventMask);
    int  readLeBufferSize(uint16_t& length, uint8_t& maxPacket);
    int  setLeRandomAddress(uint8_t addr[6]);
    int  setLeAdvertisingParams(
         uint16_t minInterval, uint16_t maxInterval, uint8_t advType, uint8_t ownBDAddrType,
         uint8_t directBDAddrType, uint8_t directBDAddr[6],
         uint8_t chanMap, uint8_t filter
    );
    int  setLeAdvertisingData(uint8_t length, uint8_t data[]);
    int  setLeScanResponseData(uint8_t length, uint8_t data[]);
    int  setLeAdvertiseEnable(uint8_t enable);
    int  updateLeConnection(
         uint16_t handle,  uint16_t minInterval, uint16_t maxInterval,
         uint16_t latency, uint16_t supervisionTimeout
    );
    int  sendAclPacket(uint16_t handle, uint8_t cid, uint8_t length, void* data);
    int  disconnect(uint16_t handle);
private:
    int  sendCommand(uint16_t opcode, uint8_t length = 0, void* params = NULL);
    void handleAclDataPacket(uint8_t length, uint8_t packet[]);
    void handleNumCompPackets(uint16_t handle, uint16_t numPackets);
    void handleEventPacket(uint8_t length, uint8_t packet[]);
    void dumpPacket(const char* prefix, uint8_t length, uint8_t packet[]);
    int  m_recvIndex;
    uint8_t  m_recvBuffer[3 + 255];
    uint16_t m_cmdCompleteOpcode;
    int  m_cmdCompleteStatus;
    uint8_t  m_cmdResponseLength;
    uint8_t* m_cmdResponse;
    uint8_t  m_maxPacket;
    uint8_t  m_pendingPacket;
};

extern HCIClass x_hci;

// HCI PACKET TYPE
#define HCI_COMMAND_PKT        0x01
#define HCI_ACLDATA_PKT        0x02
#define HCI_EVENT_PKT          0x04

// HCI EVENT
#define EVT_CMD_COMPLETE       0x0e
#define EVT_DISCONN_COMPLETE   0x05
#define EVT_NUM_COMP_PKTS      0x13
#define EVT_LE_META_EVENT      0x3e
#define EVT_LE_CONN_COMPLETE   0x01

// OGF
#define OGF_LINK_CTL           0x01
#define OGF_HOST_CTL           0x03
#define OGF_INFO_PARAM         0x04
#define OGF_STATUS_PARAM       0x05
#define OGF_LE_CTL             0x08

// OCF
// OGF_LINK_CTL
#define OCF_DISCONNECT         0x0006
// OGF_HOST_CTL
#define OCF_SET_EVENT_MASK     0x0001
#define OCF_RESET              0x0003
// OGF_INFO_PARAM
#define OCF_READ_LOCAL_VERSION 0x0001
#define OCF_READ_BD_ADDR       0x0009
// OGF_STATUS_PARAM
#define OCF_READ_RSSI          0x0005
// OGF_LE_CTL
#define OCF_LE_READ_BUFFER_SIZE           0x0002
#define OCF_LE_SET_RANDOM_ADDRESS         0x0005
#define OCF_LE_SET_ADVERTISING_PARAMETERS 0x0006
#define OCF_LE_SET_ADVERTISING_DATA       0x0008
#define OCF_LE_SET_SCAN_RESPONSE_DATA     0x0009
#define OCF_LE_SET_ADVERTISE_ENABLE       0x000a
#define OCF_LE_CONN_UPDATE                0x0013

#define HCI_OE_USER_ENDED_CONNECTION      0x13

#endif /* HCI_H_ */