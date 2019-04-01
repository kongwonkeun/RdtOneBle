/*
 * ATT.h
 *
 * Created: 2019-03-27 10:34:10
 *  Author: kong
 */

#ifndef ATT_H_
#define ATT_H_

#include <stdint.h>
#include "profile/BLEDevice.h"

class ATTClass
{
public:
    ATTClass();
    virtual ~ATTClass();
    void setMaxMtu(uint16_t maxMtu);
    void addConnection(
         uint16_t handle, uint8_t role, uint8_t peerBDAddrType, uint8_t peerBDAddr[6],
         uint16_t interval, uint16_t latency, uint16_t supervisionTimeout, uint8_t masterClockAccuracy
    );
    void handleData(uint16_t handle, uint8_t length, uint8_t data[]);
    void removeConnection(uint8_t handle, uint16_t reason);
    bool connected();
    bool connected(uint16_t handle, const uint8_t address[6]);
    bool disconnect();
    bool handleNotify(uint16_t handle, const uint8_t* value, int length);
    bool handleInd(uint16_t handle, const uint8_t* value, int length);
    void setEventHandler(BLEDeviceEvent event, BLEDeviceEventHandler eventHandler);
    BLEDevice central();
private:
    void mtuReq(uint16_t handle, uint8_t length, uint8_t data[]);
    void findInfoReq(uint16_t handle, uint8_t length, uint8_t data[]);
    void findByTypeReq(uint16_t handle, uint8_t length, uint8_t data[]);
    void readByTypeReq(uint16_t handle, uint8_t length, uint8_t data[]);
    void readOrReadBlobReq(uint16_t handle, uint8_t opcode, uint8_t length, uint8_t data[]);
    void readByGroupReq(uint16_t handle, uint8_t length, uint8_t data[]);
    void writeReqOrCmd(uint16_t handle, uint8_t opcode, uint8_t length, uint8_t data[]);
    void prepWriteReq(uint16_t handle, uint8_t length, uint8_t data[]);
    void execWriteReq(uint16_t handle, uint8_t length, uint8_t data[]);
    void handleCnf(uint16_t handle, uint8_t length, uint8_t data[]);
    void sendError(uint16_t handle, uint8_t opcode, uint16_t errHandle, uint8_t code);
    uint16_t m_maxMtu;
    uint16_t m_connectionHandle;
    uint16_t m_mtu;
    uint16_t m_longWriteHandle;
    uint8_t* m_longWriteValue;
    uint16_t m_longWriteValueLength;
    uint8_t  m_peerAddress[6];
    volatile bool m_cnf;
    BLEDeviceEventHandler m_eventHandlers[BD_EVENT_LAST];
};

extern ATTClass g_att;

#define ATT_CID 0x0004

#define ATT_OP_ERROR              0x01
#define ATT_OP_MTU_REQ            0x02
#define ATT_OP_MTU_RESP           0x03
#define ATT_OP_FIND_INFO_REQ      0x04
#define ATT_OP_FIND_INFO_RESP     0x05
#define ATT_OP_FIND_BY_TYPE_REQ   0x06
#define ATT_OP_FIND_BY_TYPE_RESP  0x07
#define ATT_OP_READ_BY_TYPE_REQ   0x08
#define ATT_OP_READ_BY_TYPE_RESP  0x09
#define ATT_OP_READ_REQ           0x0a
#define ATT_OP_READ_RESP          0x0b
#define ATT_OP_READ_BLOB_REQ      0x0c
#define ATT_OP_READ_BLOB_RESP     0x0d
#define ATT_OP_READ_MULTI_REQ     0x0e
#define ATT_OP_READ_MULTI_RESP    0x0f
#define ATT_OP_READ_BY_GROUP_REQ  0x10
#define ATT_OP_READ_BY_GROUP_RESP 0x11
#define ATT_OP_WRITE_REQ          0x12
#define ATT_OP_WRITE_RESP         0x13
#define ATT_OP_WRITE_CMD          0x52
#define ATT_OP_PREP_WRITE_REQ     0x16
#define ATT_OP_PREP_WRITE_RESP    0x17
#define ATT_OP_EXEC_WRITE_REQ     0x18
#define ATT_OP_EXEC_WRITE_RESP    0x19
#define ATT_OP_HANDLE_NOTIFY      0x1b
#define ATT_OP_HANDLE_IND         0x1d
#define ATT_OP_HANDLE_CNF         0x1e
#define ATT_OP_SIGNED_WRITE_CMD   0xd2

#define ATT_ECODE_INVALID_HANDLE       0x01
#define ATT_ECODE_READ_NOT_PERM        0x02
#define ATT_ECODE_WRITE_NOT_PERM       0x03
#define ATT_ECODE_INVALID_PDU          0x04
#define ATT_ECODE_AUTHENTICATION       0x05
#define ATT_ECODE_REQ_NOT_SUPP         0x06
#define ATT_ECODE_INVALID_OFFSET       0x07
#define ATT_ECODE_AUTHORIZATION        0x08
#define ATT_ECODE_PREP_QUEUE_FULL      0x09
#define ATT_ECODE_ATTR_NOT_FOUND       0x0a
#define ATT_ECODE_ATTR_NOT_LONG        0x0b
#define ATT_ECODE_INSUFF_ENCR_KEY_SIZE 0x0c
#define ATT_ECODE_INVAL_ATTR_VALUE_LEN 0x0d
#define ATT_ECODE_UNLIKELY             0x0e
#define ATT_ECODE_INSUFF_ENC           0x0f
#define ATT_ECODE_UNSUPP_GRP_TYPE      0x10
#define ATT_ECODE_INSUFF_RESOURCES     0x11

#endif /* ATT_H_ */