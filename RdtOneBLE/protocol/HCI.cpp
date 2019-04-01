/*
 * HCI.cpp
 *
 * Created: 2019-03-29 11:17:17
 *  Author: kong
 */

#include <string.h>
#include "system/XUtil.h"
#include "system/XTimer.h"
#include "protocol/HCITransport.h"
#include "protocol/L2CAPSignaling.h"
#include "protocol/ATT.h"
#include "protocol/HCI.h"

HCIClass::HCIClass() : m_recvIndex(0), m_pendingPacket(0)
{
    // just initialize properties
}

HCIClass::~HCIClass()
{
    // nothing to do
}

int HCIClass::begin()
{
    m_recvIndex = 0;
    return g_hciTransport.begin();
}

void HCIClass::end()
{
    g_hciTransport.end();
}

void HCIClass::poll()
{
    poll(0);
}

void HCIClass::poll(unsigned long timeout)
{
    if (timeout) {
        g_hciTransport.wait(timeout);
    }
    while (g_hciTransport.available()) {
        byte b = g_hciTransport.read();
        m_recvBuffer[m_recvIndex++] = b;

        if (m_recvBuffer[0] == HCI_ACLDATA_PKT) {
            if (m_recvIndex > 5 && m_recvIndex >= (5 + (m_recvBuffer[3] + (m_recvBuffer[4] << 8)))) {
                int pktLen = m_recvIndex - 1;
                m_recvIndex = 0;
                handleAclDataPacket(pktLen, &m_recvBuffer[1]);
            }
        }
        else if (m_recvBuffer[0] == HCI_EVENT_PKT) {
            if (m_recvIndex > 3 && m_recvIndex >= (3 + m_recvBuffer[2])) {
                // received full event
                int pktLen = m_recvIndex - 1;
                m_recvIndex = 0;
                handleEventPacket(pktLen, &m_recvBuffer[1]);
            }
        } else {
            m_recvIndex = 0;
        }
    }
}

int HCIClass::reset()
{
    return sendCommand(OGF_HOST_CTL << 10 | OCF_RESET);
}

int HCIClass::readLocalVersion(uint8_t& hciVer, uint16_t& hciRev, uint8_t& lmpVer, uint16_t& manufacturer, uint16_t& lmpSubVer)
{
    int result = sendCommand(OGF_INFO_PARAM << 10 | OCF_READ_LOCAL_VERSION);
    if (result == 0) {
        struct __attribute__ ((packed)) HCILocalVersion {
            uint8_t  hciVer;
            uint16_t hciRev;
            uint8_t  lmpVer;
            uint16_t manufacturer;
            uint16_t lmpSubVer;
        } *localVersion = (HCILocalVersion*)m_cmdResponse;

        hciVer = localVersion->hciVer;
        hciRev = localVersion->hciRev;
        lmpVer = localVersion->lmpVer;
        manufacturer = localVersion->manufacturer;
        lmpSubVer = localVersion->lmpSubVer;
    }
    return result;
}

int HCIClass::readBDAddr(uint8_t addr[6])
{
    int result = sendCommand(OGF_INFO_PARAM << 10 | OCF_READ_BD_ADDR);
    if (result == 0) {
        memcpy(addr, m_cmdResponse, 6);
    }
    return result;
}

int HCIClass::readRssi(uint16_t handle)
{
    int result = sendCommand(OGF_STATUS_PARAM << 10 | OCF_READ_RSSI, sizeof(handle), &handle);
    int rssi = 127;
    if (result == 0) {
        struct __attribute__ ((packed)) HCIReadRssi {
            uint16_t handle;
            int8_t   rssi;
        } *readRssi = (HCIReadRssi*)m_cmdResponse;

        if (readRssi->handle == handle) {
            rssi = readRssi->rssi;
        }
    }
    return rssi;
}

int HCIClass::setEventMask(uint64_t eventMask)
{
    return sendCommand(OGF_HOST_CTL << 10 | OCF_SET_EVENT_MASK, sizeof(eventMask), &eventMask);
}

int HCIClass::readLeBufferSize(uint16_t& pktLen, uint8_t& maxPkt)
{
    int result = sendCommand(OGF_LE_CTL << 10 | OCF_LE_READ_BUFFER_SIZE);
    if (result == 0) {
        struct __attribute__ ((packed)) HCILeBufferSize {
            uint16_t pktLen;
            uint8_t  maxPkt;
        } *leBufferSize = (HCILeBufferSize*)m_cmdResponse;

        pktLen = leBufferSize->pktLen;
        m_maxPacket = maxPkt = leBufferSize->maxPkt;
        #ifndef __AVR__
        g_att.setMaxMtu(pktLen - 9); // max pkt len - ACL header size
        #endif
    }
    return result;
}

int HCIClass::setLeRandomAddress(uint8_t addr[6])
{
    return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_RANDOM_ADDRESS, 6, addr);
}

int HCIClass::setLeAdvertisingParams(uint16_t minInterval, uint16_t maxInterval, uint8_t advType,
uint8_t ownBDAddrType, uint8_t directBDAddrType, uint8_t directBDAddr[6], uint8_t chanMap, uint8_t filter)
{
    struct __attribute__ ((packed)) HCILeAdvertisingParameters {
        uint16_t minInterval;
        uint16_t maxInterval;
        uint8_t  advType;
        uint8_t  ownBDAddrType;
        uint8_t  directBDAddrType;
        uint8_t  directBDAddr[6];
        uint8_t  chanMap;
        uint8_t  filter;
    } leAdvertisingParamters;

    leAdvertisingParamters.minInterval = minInterval;
    leAdvertisingParamters.maxInterval = maxInterval;
    leAdvertisingParamters.advType = advType;
    leAdvertisingParamters.ownBDAddrType = ownBDAddrType;
    leAdvertisingParamters.directBDAddrType = directBDAddrType;
    memcpy(leAdvertisingParamters.directBDAddr, directBDAddr, 6);
    leAdvertisingParamters.chanMap = chanMap;
    leAdvertisingParamters.filter = filter;
    return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_ADVERTISING_PARAMETERS, sizeof(leAdvertisingParamters), &leAdvertisingParamters);
}

int HCIClass::setLeAdvertisingData(uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) HCILeAdvertisingData {
        uint8_t length;
        uint8_t data[31];
    } leAdvertisingData;

    memset(&leAdvertisingData, 0, sizeof(leAdvertisingData));
    leAdvertisingData.length = length;
    memcpy(leAdvertisingData.data, data, length);
    return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_ADVERTISING_DATA, sizeof(leAdvertisingData), &leAdvertisingData);
}

int HCIClass::setLeScanResponseData(uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) HCILeScanResponseData {
        uint8_t length;
        uint8_t data[31];
    } leScanResponseData;

    memset(&leScanResponseData, 0, sizeof(leScanResponseData));
    leScanResponseData.length = length;
    memcpy(leScanResponseData.data, data, length);
    return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_SCAN_RESPONSE_DATA, sizeof(leScanResponseData), &leScanResponseData);
}

int HCIClass::setLeAdvertiseEnable(uint8_t enable)
{
    return sendCommand(OGF_LE_CTL << 10 | OCF_LE_SET_ADVERTISE_ENABLE, sizeof(enable), &enable);
}

int HCIClass::updateLeConnection(uint16_t handle, uint16_t minInterval, uint16_t maxInterval,
uint16_t latency, uint16_t supervisionTimeout)
{
    struct __attribute__ ((packed)) HCILeConnUpdateData {
        uint16_t handle;
        uint16_t minInterval;
        uint16_t maxInterval;
        uint16_t latency;
        uint16_t supervisionTimeout;
        uint16_t minCeLength;
        uint16_t maxCeLength;
    } leConnUpdateData;

    leConnUpdateData.handle = handle;
    leConnUpdateData.minInterval = minInterval;
    leConnUpdateData.maxInterval = maxInterval;
    leConnUpdateData.latency = latency;
    leConnUpdateData.supervisionTimeout = supervisionTimeout;
    leConnUpdateData.minCeLength = 0x0004;
    leConnUpdateData.maxCeLength = 0x0006;
    return sendCommand(OGF_LE_CTL << 10 | OCF_LE_CONN_UPDATE, sizeof(leConnUpdateData), &leConnUpdateData);
}

int HCIClass::sendAclPacket(uint16_t handle, uint8_t cid, uint8_t plen, void* data)
{
    while (m_pendingPacket >= m_maxPacket) {
        poll();
    }
    struct __attribute__ ((packed)) HCIACLHdr {
        uint8_t  pktType;
        uint16_t handle;
        uint16_t dlen;
        uint16_t plen;
        uint16_t cid;
    } aclHdr = { HCI_ACLDATA_PKT, handle, uint8_t(plen + 4), plen, cid };

    uint8_t txBuffer[sizeof(aclHdr) + plen];
    memcpy(txBuffer, &aclHdr, sizeof(aclHdr));
    memcpy(&txBuffer[sizeof(aclHdr)], data, plen);
    m_pendingPacket++;
    g_hciTransport.write(txBuffer, sizeof(aclHdr) + plen);
    return 0;
}

int HCIClass::disconnect(uint16_t handle)
{
    struct __attribute__ ((packed)) HCIDisconnectData {
        uint16_t handle;
        uint8_t  reason;
    } disconnectData = { handle, HCI_OE_USER_ENDED_CONNECTION };

    return sendCommand(OGF_LINK_CTL << 10 | OCF_DISCONNECT, sizeof(disconnectData), &disconnectData);
}

int HCIClass::sendCommand(uint16_t opcode, uint8_t plen, void* parameters)
{
    struct __attribute__ ((packed)) {
        uint8_t  pktType;
        uint16_t opcode;
        uint8_t  plen;
    } pktHdr = { HCI_COMMAND_PKT, opcode, plen };

    uint8_t txBuffer[sizeof(pktHdr) + plen];
    memcpy(txBuffer, &pktHdr, sizeof(pktHdr));
    memcpy(&txBuffer[sizeof(pktHdr)], parameters, plen);
    g_hciTransport.write(txBuffer, sizeof(pktHdr) + plen);
    m_cmdCompleteOpcode = 0xffff;
    m_cmdCompleteStatus = -1;
    for (unsigned long start = millis(); m_cmdCompleteOpcode != opcode && millis() < (start + 1000); ) {
        poll();
    }
    return m_cmdCompleteStatus;
}

void HCIClass::handleAclDataPacket(uint8_t, uint8_t pdata[])
{
    struct __attribute__ ((packed)) HCIACLHdr {
        uint16_t handle;
        uint16_t dlen;
        uint16_t len;
        uint16_t cid;
    } *aclHdr = (HCIACLHdr*)pdata;

    if (aclHdr->cid == ATT_CID) {
        g_att.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &m_recvBuffer[1 + sizeof(HCIACLHdr)]);
    }
    else if (aclHdr->cid == SIGNALING_CID) {
        g_l2capSignaling.handleData(aclHdr->handle & 0x0fff, aclHdr->len, &m_recvBuffer[1 + sizeof(HCIACLHdr)]);
    } else {
        struct __attribute__ ((packed)) {
            uint8_t  op;
            uint8_t  id;
            uint16_t length;
            uint16_t reason;
            uint16_t localCid;
            uint16_t remoteCid;
        } l2capRejectCid= { 0x01, 0x00, 0x006, 0x0002, aclHdr->cid, 0x0000 };

        sendAclPacket(aclHdr->handle & 0x0fff, 0x0005, sizeof(l2capRejectCid), &l2capRejectCid);
    }
}

void HCIClass::handleNumCompPackets(uint16_t, uint16_t numPkts)
{
    if (numPkts && m_pendingPacket > numPkts) {
        m_pendingPacket -= numPkts;
    } else {
        m_pendingPacket = 0;
    }
}

void HCIClass::handleEventPacket(uint8_t, uint8_t pdata[])
{
    struct __attribute__ ((packed)) HCIEventHdr {
        uint8_t evt;
        uint8_t plen;
    } *eventHdr = (HCIEventHdr*)pdata;

    if (eventHdr->evt == EVT_DISCONN_COMPLETE) {
        struct __attribute__ ((packed)) DisconnComplete {
            uint8_t  status;
            uint16_t handle;
            uint8_t  reason;
        } *disconnComplete = (DisconnComplete*)&pdata[sizeof(HCIEventHdr)];

        g_att.removeConnection(disconnComplete->handle, disconnComplete->reason);
        g_l2capSignaling.removeConnection(disconnComplete->handle, disconnComplete->reason);
        g_hci.setLeAdvertiseEnable(0x01);
    }
    else if (eventHdr->evt == EVT_CMD_COMPLETE) {
        struct __attribute__ ((packed)) CmdComplete {
            uint8_t  ncmd;
            uint16_t opcode;
            uint8_t  status;
        } *cmdCompleteHeader = (CmdComplete*)&pdata[sizeof(HCIEventHdr)];

        m_cmdCompleteOpcode = cmdCompleteHeader->opcode;
        m_cmdCompleteStatus = cmdCompleteHeader->status;
        m_cmdResponseLength = pdata[1] - sizeof(CmdComplete);
        m_cmdResponse = &pdata[sizeof(HCIEventHdr) + sizeof(CmdComplete)];
    }
    else if (eventHdr->evt == EVT_NUM_COMP_PKTS) {
        uint8_t numHandles = pdata[sizeof(HCIEventHdr)];
        uint16_t* data = (uint16_t*)&pdata[sizeof(HCIEventHdr) + sizeof(numHandles)];

        for (uint8_t i = 0; i < numHandles; i++) {
            handleNumCompPackets(data[0], data[1]);
            data += 2;
        }
    }
    else if (eventHdr->evt == EVT_LE_META_EVENT) {
        struct __attribute__ ((packed)) LeMetaEventHeader {
            uint8_t subevent;
        } *leMetaHeader = (LeMetaEventHeader*)&pdata[sizeof(HCIEventHdr)];

        if (leMetaHeader->subevent == EVT_LE_CONN_COMPLETE) {
            struct __attribute__ ((packed)) EvtLeConnectionComplete {
                uint8_t  status;
                uint16_t handle;
                uint8_t  role;
                uint8_t  peerBdaddrType;
                uint8_t  peerBdaddr[6];
                uint16_t interval;
                uint16_t latency;
                uint16_t supervisionTimeout;
                uint8_t  masterClockAccuracy;
            } *leConnectionComplete = (EvtLeConnectionComplete*)&pdata[sizeof(HCIEventHdr) + sizeof(LeMetaEventHeader)];

            if (leConnectionComplete->status == 0x00) {
                g_att.addConnection(
                    leConnectionComplete->handle,
                    leConnectionComplete->role,
                    leConnectionComplete->peerBdaddrType,
                    leConnectionComplete->peerBdaddr,
                    leConnectionComplete->interval,
                    leConnectionComplete->latency,
                    leConnectionComplete->supervisionTimeout,
                    leConnectionComplete->masterClockAccuracy
                );
                g_l2capSignaling.addConnection(
                    leConnectionComplete->handle,
                    leConnectionComplete->role,
                    leConnectionComplete->peerBdaddrType,
                    leConnectionComplete->peerBdaddr,
                    leConnectionComplete->interval,
                    leConnectionComplete->latency,
                    leConnectionComplete->supervisionTimeout,
                    leConnectionComplete->masterClockAccuracy
                );
            }
        }
    }
}

void HCIClass::dumpPacket(const char* prefix, uint8_t plen, uint8_t pdata[])
{
    for (uint8_t i = 0; i < plen; i++) {
        //byte b = pdata[i];
        // to do something
    }
}

HCIClass g_hci;

/* EOF */