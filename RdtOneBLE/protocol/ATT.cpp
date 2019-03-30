/*
 * ATT.cpp
 *
 * Created: 2019-03-27 15:51:40
 *  Author: kong
 */


#include "../protocol/ATT.h"
#include "../protocol/GATT.h"
#include "../protocol/HCI.h"
#include "../profile/BLEProperty.h"
#include "../profile/BLEDescriptor.h"
#include "../peripheral/LocalDescriptor.h"
#include "../system/XUtil.h"
#include <string.h>

class BLEDevice;
class BLEAttribute;
class LocalCharacteristic;
class LocalService;

ATTClass::ATTClass() : m_maxMtu(23), m_connectionHandle(0xffff), m_mtu(23), m_longWriteHandle(0x0000), m_longWriteValue(NULL), m_longWriteValueLength(0)
{
    memset(m_peerAddress, 0x00, sizeof(m_peerAddress));
    memset(m_eventHandlers, 0x00, sizeof(m_eventHandlers));
}

ATTClass::~ATTClass()
{
    if (m_longWriteValue) {
        free(m_longWriteValue);
    }
}

void ATTClass::setMaxMtu(uint16_t maxMtu)
{
    m_maxMtu = maxMtu;
}

void ATTClass::addConnection(uint16_t handle, uint8_t role, uint8_t, uint8_t peerBDAddr[6], uint16_t, uint16_t, uint16_t, uint8_t)
{
    if (role == 1) {
        m_connectionHandle = handle;
        m_mtu = 23;
        memcpy(m_peerAddress, peerBDAddr, sizeof(m_peerAddress));
        if (m_eventHandlers[BD_CONNECTED]) {
            m_eventHandlers[BD_CONNECTED](BLEDevice(m_connectionHandle, m_peerAddress));
        }
    }
}

void ATTClass::handleData(uint16_t handle, uint8_t length, uint8_t data[])
{
    uint8_t opcode = data[0];
    length--;
    data++;
    switch (opcode) {
        case ATT_OP_MTU_REQ: mtuReq(handle, length, data); break;
        case ATT_OP_FIND_INFO_REQ: findInfoReq(handle, length, data); break;
        case ATT_OP_FIND_BY_TYPE_REQ: findByTypeReq(handle, length, data); break;
        case ATT_OP_READ_BY_TYPE_REQ: readByTypeReq(handle, length, data); break;
        case ATT_OP_READ_BY_GROUP_REQ: readByGroupReq(handle, length, data); break;
        case ATT_OP_READ_REQ:
        case ATT_OP_READ_BLOB_REQ: readOrReadBlobReq(handle, opcode, length, data); break;
        case ATT_OP_WRITE_REQ:
        case ATT_OP_WRITE_CMD: writeReqOrCmd(handle, opcode, length, data); break;
        case ATT_OP_PREP_WRITE_REQ: prepWriteReq(handle, length, data); break;
        case ATT_OP_EXEC_WRITE_REQ: execWriteReq(handle, length, data); break;
        case ATT_OP_HANDLE_CNF: handleCnf(handle, length, data); break;
        case ATT_OP_READ_MULTI_REQ:
        case ATT_OP_SIGNED_WRITE_CMD:
        default: sendError(handle, opcode, 0x0000, ATT_ECODE_REQ_NOT_SUPP); break;
    }
}

void ATTClass::removeConnection(uint8_t handle, uint16_t)
{
    if (m_connectionHandle == handle) {
        BLEDevice bleDevice(m_connectionHandle, m_peerAddress);
        for (uint16_t i = 0; i < g_gatt.attributeCount(); i++) {
            BLEAttribute* attribute = g_gatt.attribute(i);
            if (attribute->type() == BATT_CHRACTERISTIC) {
                LocalCharacteristic* characteristic = (LocalCharacteristic*)attribute;
                characteristic->writeCccdValue(bleDevice, 0x0000);
            }
        }
        if (m_eventHandlers[BD_DISCONNECTED]) {
            m_eventHandlers[BD_DISCONNECTED](bleDevice);
        }
        m_connectionHandle = 0xffff;
        memset(m_peerAddress, 0x00, sizeof(m_peerAddress));
        m_longWriteHandle = 0x0000;
        m_longWriteValueLength = 0;
    }
}

bool ATTClass::connected()
{
    return (m_connectionHandle != 0xffff);
}

bool ATTClass::connected(uint16_t handle, const uint8_t address[6])
{
    return ((m_connectionHandle == handle) && memcmp(m_peerAddress, address, 6) == 0);
}

bool ATTClass::disconnect()
{
    if (m_connectionHandle != 0xffff) {
        if (g_hci.disconnect(m_connectionHandle) != 0) {
            return false;
        }
        m_connectionHandle = 0xffff;
        memset(m_peerAddress, 0x00, sizeof(m_peerAddress));
        return true;
    }
    return false;
}

BLEDevice ATTClass::central()
{
    if (connected()) {
        return BLEDevice(m_connectionHandle, m_peerAddress);
    }
    return BLEDevice();
}

bool ATTClass::handleNotify(uint16_t handle, const uint8_t* value, int length)
{
    if (m_connectionHandle != 0xffff) {
        uint8_t  notication[m_mtu];
        uint16_t noticationLength = 0;
        notication[0] = ATT_OP_HANDLE_NOTIFY;
        noticationLength++;
        memcpy(&notication[1], &handle, sizeof(handle));
        noticationLength += sizeof(handle);
        length = min((uint16_t)(m_mtu - noticationLength), (uint16_t)length);
        memcpy(&notication[noticationLength], value, length);
        noticationLength += length;
        g_hci.sendAclPacket(m_connectionHandle, ATT_CID, noticationLength, notication);
        return true;
    }
    return false;
}

bool ATTClass::handleInd(uint16_t handle, const uint8_t* value, int length)
{
    if (m_connectionHandle != 0xffff) {
        uint8_t  indication[m_mtu];
        uint16_t indicationLength = 0;
        indication[0] = ATT_OP_HANDLE_IND;
        indicationLength++;
        memcpy(&indication[1], &handle, sizeof(handle));
        indicationLength += sizeof(handle);
        length = min((uint16_t)(m_mtu - indicationLength), (uint16_t)length);
        memcpy(&indication[indicationLength], value, length);
        indicationLength += length;

        m_cnf = false;
        g_hci.sendAclPacket(m_connectionHandle, ATT_CID, indicationLength, indication);
        while (!m_cnf) {
            g_hci.poll();
            if (!connected()) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void ATTClass::mtuReq(uint16_t handle, uint8_t length, uint8_t data[])
{
    uint16_t mtu = *(uint16_t*)data;
    if (length != 2) {
        sendError(handle, ATT_OP_MTU_REQ, 0x0000, ATT_ECODE_INVALID_PDU);
        return;
    }
    if (mtu > m_maxMtu) {
        mtu = m_maxMtu;
    }
    m_mtu = mtu;

    struct __attribute__ ((packed)) {
        uint8_t  op;
        uint16_t mtu;
    } mtuResp = { ATT_OP_MTU_RESP, mtu };

    g_hci.sendAclPacket(handle, ATT_CID, sizeof(mtuResp), &mtuResp);
}

void ATTClass::findInfoReq(uint16_t handle, uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) FindInfoReq {
        uint16_t startHandle;
        uint16_t endHandle;
    } *findInfoReq = (FindInfoReq*)data;

    if (length != sizeof(FindInfoReq)) {
        sendError(handle, ATT_OP_FIND_INFO_REQ, findInfoReq->startHandle, ATT_ECODE_INVALID_PDU);
        return;
    }
    uint8_t  response[m_mtu];
    uint16_t responseLength;
    response[0] = ATT_OP_FIND_INFO_RESP;
    response[1] = 0x00;
    responseLength = 2;

    for (uint16_t i = (findInfoReq->startHandle - 1); i < g_gatt.attributeCount() && i <= (findInfoReq->endHandle - 1); i++) {
        BLEAttribute* attribute = g_gatt.attribute(i);
        uint16_t h = (i + 1);
        bool isValueHandle = (attribute->type() == BATT_CHRACTERISTIC) && (((LocalCharacteristic*)attribute)->valueHandle() == h);
        int uuidLen = isValueHandle ? 2 : attribute->uuidLength();
        int infoType = (uuidLen == 2) ? 0x01 : 0x02;

        if (response[1] == 0) {
            response[1] = infoType;
        }
        if (response[1] != infoType) {
            // different type
            break;
        }
        // add the handle
        memcpy(&response[responseLength], &h, sizeof(h));
        responseLength += sizeof(h);
        if (isValueHandle || attribute->type() == BATT_DESCRIPTOR) {
            // add the UUID
            memcpy(&response[responseLength], attribute->uuidData(), uuidLen);
            responseLength += uuidLen;
        } else {
            // add the type
            uint16_t type = attribute->type();
            memcpy(&response[responseLength], &type, sizeof(type));
            responseLength += sizeof(type);
        }
        if ((responseLength + (2 + uuidLen)) > m_mtu) {
            break;
        }
    }
    if (responseLength == 2) {
        sendError(handle, ATT_OP_FIND_INFO_REQ, findInfoReq->startHandle, ATT_ECODE_ATTR_NOT_FOUND);
    } else {
        g_hci.sendAclPacket(handle, ATT_CID, responseLength, response);
    }
}

void ATTClass::findByTypeReq(uint16_t handle, uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) FindByTypeReq {
        uint16_t startHandle;
        uint16_t endHandle;
        uint16_t type;
    } *findByTypeReq = (FindByTypeReq*)data;

    if (length < sizeof(FindByTypeReq)) {
        sendError(handle, ATT_OP_FIND_BY_TYPE_RESP, findByTypeReq->startHandle, ATT_ECODE_INVALID_PDU);
        return;
    }
    uint16_t valueLength = length - sizeof(*findByTypeReq);
    uint8_t* value = &data[sizeof(*findByTypeReq)];
    uint8_t  response[m_mtu];
    uint16_t responseLength;
    response[0] = ATT_OP_FIND_BY_TYPE_RESP;
    responseLength = 1;

    if (findByTypeReq->type == BATT_SERVICE) {
        for (uint16_t i = (findByTypeReq->startHandle - 1); i < g_gatt.attributeCount() && i <= (findByTypeReq->endHandle - 1); i++) {
            BLEAttribute* attribute = g_gatt.attribute(i);
            if ((attribute->type() == findByTypeReq->type) && (attribute->uuidLength() == valueLength) && memcmp(attribute->uuidData(), value, valueLength) == 0) {
                LocalService* service = (LocalService*)attribute;
                // add the start handle
                uint16_t startHandle = service->startHandle();
                memcpy(&response[responseLength], &startHandle, sizeof(startHandle));
                responseLength += sizeof(startHandle);
                // add the end handle
                uint16_t endHandle = service->endHandle();
                memcpy(&response[responseLength], &endHandle, sizeof(endHandle));
                responseLength += sizeof(endHandle);
            }
            if ((responseLength + 4) > m_mtu) {
                break;
            }
        }
    }
    if (responseLength == 1) {
        sendError(handle, ATT_OP_FIND_BY_TYPE_RESP, findByTypeReq->startHandle, ATT_ECODE_ATTR_NOT_FOUND);
    } else {
        g_hci.sendAclPacket(handle, ATT_CID, responseLength, response);
    }
}

void ATTClass::readByGroupReq(uint16_t handle, uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) ReadByGroupReq {
        uint16_t startHandle;
        uint16_t endHandle;
        uint16_t uuid;
    } *readByGroupReq = (ReadByGroupReq*)data;

    if (length != sizeof(ReadByGroupReq) || (readByGroupReq->uuid != BATT_SERVICE && readByGroupReq->uuid != 0x2801)) {
        sendError(handle, ATT_OP_READ_BY_GROUP_REQ, readByGroupReq->startHandle, ATT_ECODE_UNSUPP_GRP_TYPE);
        return;
    }
    uint8_t  response[m_mtu];
    uint16_t responseLength;
    response[0] = ATT_OP_READ_BY_GROUP_RESP;
    response[1] = 0x00;
    responseLength = 2;

    for (uint16_t i = (readByGroupReq->startHandle - 1); i < g_gatt.attributeCount() && i <= (readByGroupReq->endHandle - 1); i++) {
        BLEAttribute* attribute = g_gatt.attribute(i);
        if (readByGroupReq->uuid != attribute->type()) {
            // not the type
            continue;
        }
        int uuidLen = attribute->uuidLength();
        int infoSize = (uuidLen == 2) ? 6 : 20;
        if (response[1] == 0) {
            response[1] = infoSize;
        }
        if (response[1] != infoSize) {
            // different size
            break;
        }
        LocalService* service = (LocalService*)attribute;
        // add the start handle
        uint16_t startHandle = service->startHandle();
        memcpy(&response[responseLength], &startHandle, sizeof(startHandle));
        responseLength += sizeof(startHandle);
        // add the end handle
        uint16_t endHandle = service->endHandle();
        memcpy(&response[responseLength], &endHandle, sizeof(endHandle));
        responseLength += sizeof(endHandle);
        // add the UUID
        memcpy(&response[responseLength], service->uuidData(), uuidLen);
        responseLength += uuidLen;
        if ((responseLength + infoSize) > m_mtu) {
            break;
        }
    }
    if (responseLength == 2) {
        sendError(handle, ATT_OP_READ_BY_GROUP_REQ, readByGroupReq->startHandle, ATT_ECODE_ATTR_NOT_FOUND);
    } else {
        g_hci.sendAclPacket(handle, ATT_CID, responseLength, response);
    }
}

void ATTClass::readOrReadBlobReq(uint16_t handle, uint8_t opcode, uint8_t length, uint8_t data[])
{
    if (opcode == ATT_OP_READ_REQ) {
        if (length != sizeof(uint16_t)) {
            sendError(handle, ATT_OP_READ_REQ, 0x0000, ATT_ECODE_INVALID_PDU);
            return;
        }
    } else {
        if (length != (sizeof(uint16_t) + sizeof(uint16_t))) {
            sendError(handle, ATT_OP_READ_BLOB_REQ, 0x0000, ATT_ECODE_INVALID_PDU);
            return;
        }
    }
    uint16_t h = *(uint16_t*)data;
    uint16_t offset = (opcode == ATT_OP_READ_REQ) ? 0 : *(uint16_t*)&data[sizeof(h)];
    if ((uint16_t)(h - 1) > g_gatt.attributeCount()) {
        sendError(handle, opcode, h, ATT_ECODE_ATTR_NOT_FOUND);
        return;
    }
    uint8_t  response[m_mtu];
    uint16_t responseLength;
    response[0] = (opcode == ATT_OP_READ_REQ) ? ATT_OP_READ_RESP : ATT_OP_READ_BLOB_RESP;
    responseLength = 1;
    BLEAttribute* attribute = g_gatt.attribute(h - 1);
    enum BLEAttributeType attributeType = attribute->type();
    if (attributeType == BATT_SERVICE) {
        if (offset) {
            sendError(handle, ATT_ECODE_ATTR_NOT_LONG, h, ATT_ECODE_INVALID_PDU);
            return;
        }
        LocalService* service = (LocalService*)attribute;
        // add the UUID
        uint8_t uuidLen = service->uuidLength();
        memcpy(&response[responseLength], service->uuidData(), uuidLen);
        responseLength += uuidLen;
    }
    else if (attributeType == BATT_CHRACTERISTIC) {
        LocalCharacteristic* characteristic = (LocalCharacteristic*)attribute;
        if (characteristic->handle() == h) {
            if (offset) {
                sendError(handle, opcode, h, ATT_ECODE_ATTR_NOT_LONG);
                return;
            }
            // add the properties
            response[responseLength++] = characteristic->properties();
            // add the value handle
            uint16_t valueHandle = characteristic->valueHandle();
            memcpy(&response[responseLength], &valueHandle, sizeof(valueHandle));
            responseLength += sizeof(valueHandle);
            // add the UUID
            uint8_t uuidLen = characteristic->uuidLength();
            memcpy(&response[responseLength], characteristic->uuidData(), uuidLen);
            responseLength += uuidLen;
       } else {
            if ((characteristic->properties() & BPP_READ) == 0) {
                sendError(handle, opcode, h, ATT_ECODE_READ_NOT_PERM);
                return;
            }
            uint16_t valueLength = characteristic->valueLength();
            if (offset >= valueLength) {
                sendError(handle, opcode, h, ATT_ECODE_INVALID_OFFSET);
                return;
            }
            valueLength = min(m_mtu - responseLength, valueLength - offset);
            characteristic->readValue(BLEDevice(handle, m_peerAddress), offset, &response[responseLength], valueLength);
            responseLength += valueLength;
        }
    }
    else if (attributeType == BATT_DESCRIPTOR) {
        LocalDescriptor* descriptor = (LocalDescriptor*)attribute;
        uint16_t valueLength = descriptor->valueSize();
        if (offset >= valueLength) {
            sendError(handle, opcode, h, ATT_ECODE_INVALID_OFFSET);
            return;
        }
        valueLength = min(m_mtu - responseLength, valueLength - offset);
        memcpy(&response[responseLength], descriptor->value() + offset, valueLength);
        responseLength += valueLength;
    }
    g_hci.sendAclPacket(handle, ATT_CID, responseLength, response);
}

void ATTClass::readByTypeReq(uint16_t handle, uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) ReadByTypeReq {
        uint16_t startHandle;
        uint16_t endHandle;
        uint16_t uuid;
    } *readByTypeReq = (ReadByTypeReq*)data;

    if (length != sizeof(ReadByTypeReq)) {
        sendError(handle, ATT_OP_READ_BY_TYPE_REQ, readByTypeReq->startHandle, ATT_ECODE_INVALID_PDU);
        return;
    }
    uint8_t  response[m_mtu];
    uint16_t responseLength;
    response[0] = ATT_OP_READ_BY_TYPE_RESP;
    response[1] = 0x00;
    responseLength = 2;
    for (uint16_t i = (readByTypeReq->startHandle - 1); i < g_gatt.attributeCount() && i <= (readByTypeReq->endHandle - 1); i++) {
        BLEAttribute* attribute = g_gatt.attribute(i);
        uint16_t h = (i + 1);
        if (attribute->type() == readByTypeReq->uuid) {
            if (attribute->type() == BATT_CHRACTERISTIC) {
                LocalCharacteristic* characteristic = (LocalCharacteristic*)attribute;
                if (characteristic->valueHandle() == h) {
                    // value handle, skip
                    continue;
                }
                int uuidLen = attribute->uuidLength();
                int typeSize = (uuidLen == 2) ? 7 : 21;
                if (response[1] == 0) {
                    response[1] = typeSize;
                }
                if (response[1] != typeSize) {
                    // all done, wrong size
                    break;
                }
                // add the handle
                memcpy(&response[responseLength], &h, sizeof(h));
                responseLength += sizeof(h);
                // add the properties
                response[responseLength++] = characteristic->properties();
                // add the value handle
                uint16_t valueHandle = (h + 1);
                memcpy(&response[responseLength], &valueHandle, sizeof(valueHandle));
                responseLength += sizeof(valueHandle);
                // add the UUID
                memcpy(&response[responseLength], characteristic->uuidData(), uuidLen);
                responseLength += uuidLen;
                // skip the next handle, it's a value handle
                i++;
                if ((responseLength + typeSize) > m_mtu) {
                    break;
                }
            }
            else if (attribute->type() == 0x2902) {
                LocalDescriptor* descriptor = (LocalDescriptor*)attribute;
                // add the handle
                memcpy(&response[responseLength], &h, sizeof(h));
                responseLength += sizeof(h);
                // add the value
                int valueSize = min((uint16_t)(m_mtu - responseLength), (uint16_t)descriptor->valueSize());
                memcpy(&response[responseLength], descriptor->value(), valueSize);
                responseLength += valueSize;
                response[1] = 2 + valueSize;
                break; // all done
            }
        }
        else if (attribute->type() == BATT_CHRACTERISTIC && attribute->uuidLength() == 2 && memcmp(&readByTypeReq->uuid, attribute->uuidData(), 2) == 0) {
            LocalCharacteristic* characteristic = (LocalCharacteristic*)attribute;
            // add the handle
            memcpy(&response[responseLength], &h, sizeof(handle));
            responseLength += sizeof(h);
            // add the value
            int valueLength = min((uint16_t)(m_mtu - responseLength), (uint16_t)characteristic->valueLength());
            memcpy(&response[responseLength], characteristic->value(), valueLength);
            responseLength += valueLength;
            response[1] = 2 + valueLength;
            break; // all done
        }
    }
    if (responseLength == 2) {
        sendError(handle, ATT_OP_READ_BY_TYPE_REQ, readByTypeReq->startHandle, ATT_ECODE_ATTR_NOT_FOUND);
    } else {
        g_hci.sendAclPacket(handle, ATT_CID, responseLength, response);
    }
}

void ATTClass::writeReqOrCmd(uint16_t handle, uint8_t opcode, uint8_t length, uint8_t data[])
{
    boolean withResponse = (opcode == ATT_OP_WRITE_REQ);
    if (length < sizeof(uint16_t)) {
        if (withResponse) {
            sendError(handle, ATT_OP_WRITE_REQ, 0x0000, ATT_ECODE_INVALID_PDU);
        }
        return;
    }
    uint16_t h = *(uint16_t*)data;
    if ((uint16_t)(h - 1) > g_gatt.attributeCount()) {
        if (withResponse) {
            sendError(handle, ATT_OP_WRITE_REQ, h, ATT_ECODE_ATTR_NOT_FOUND);
        }
        return;
    }
    uint8_t  valueLength = length - sizeof(h);
    uint8_t* value = &data[sizeof(h)];
    BLEAttribute* attribute = g_gatt.attribute(h - 1);
    if (attribute->type() == BATT_CHRACTERISTIC) {
        LocalCharacteristic* characteristic = (LocalCharacteristic*)attribute;
        
        if (h != characteristic->valueHandle() || withResponse ? 
        ((characteristic->properties() & BPP_WRITE) == 0) : 
        ((characteristic->properties() & BPP_WRITE_WO_RESPONSE) == 0)) {
            if (withResponse) {
                sendError(handle, ATT_OP_WRITE_REQ, h, ATT_ECODE_WRITE_NOT_PERM);
            }
            return;
        }
        if (handle == m_connectionHandle) {
            characteristic->writeValue(BLEDevice(handle, m_peerAddress), value, valueLength);
        }
    }
    else if (attribute->type() == BATT_DESCRIPTOR) {
        LocalDescriptor* descriptor = (LocalDescriptor*)attribute;
        // only CCCD's are writable
        if (descriptor->uuidLength() != 2 || *((uint16_t*)(descriptor->uuidData())) != 0x2902) {
            if (withResponse) {
                sendError(handle, ATT_OP_WRITE_REQ, h, ATT_ECODE_WRITE_NOT_PERM);
            }
            return;
        }
        // get the previous handle, should be the characteristic for the CCCD
        attribute = g_gatt.attribute(h - 2);
        if (attribute->type() != BATT_CHRACTERISTIC) {
            if (withResponse) {
                sendError(handle, ATT_OP_WRITE_REQ, h, ATT_ECODE_WRITE_NOT_PERM);
            }
            return;
        }
        LocalCharacteristic* characteristic = (LocalCharacteristic*)attribute;
        if (handle == m_connectionHandle) {
            characteristic->writeCccdValue(BLEDevice(handle, m_peerAddress), *((uint16_t*)value));
        }
    } else {
        if (withResponse) {
            sendError(handle, ATT_OP_WRITE_REQ, h, ATT_ECODE_WRITE_NOT_PERM);
        }
        return;
    }
    if (withResponse) {
        uint8_t  response[m_mtu];
        uint16_t responseLength;
        response[0] = ATT_OP_WRITE_RESP;
        responseLength = 1;
        g_hci.sendAclPacket(handle, ATT_CID, responseLength, response);
    }
}

void ATTClass::prepWriteReq(uint16_t handle, uint8_t length, uint8_t data[])
{
    struct __attribute__ ((packed)) PrepWriteReq {
        uint16_t handle;
        uint16_t offset;
    } *prepWriteReq = (PrepWriteReq*)data;

    if (length < sizeof(PrepWriteReq)) {
        sendError(handle, ATT_OP_PREP_WRITE_REQ, 0x0000, ATT_ECODE_INVALID_PDU);
        return;
    }
    uint16_t h = prepWriteReq->handle;
    uint16_t o = prepWriteReq->offset;
    if ((uint16_t)(h - 1) > g_gatt.attributeCount()) {
        sendError(handle, ATT_OP_PREP_WRITE_REQ, h, ATT_ECODE_ATTR_NOT_FOUND);
        return;
    }
    BLEAttribute* attribute = g_gatt.attribute(h - 1);
    if (attribute->type() != BATT_CHRACTERISTIC) {
        sendError(handle, ATT_OP_PREP_WRITE_REQ, h, ATT_ECODE_ATTR_NOT_LONG);
        return;
    }
    LocalCharacteristic* characteristic = (LocalCharacteristic*)attribute;
    if (h != characteristic->valueHandle()) {
        sendError(handle, ATT_OP_PREP_WRITE_REQ, h, ATT_ECODE_ATTR_NOT_LONG);
        return;
    }
    if ((characteristic->properties() & BPP_WRITE) == 0) {
        sendError(handle, ATT_OP_PREP_WRITE_REQ, h, ATT_ECODE_WRITE_NOT_PERM);
        return;
    }
    if (m_longWriteHandle == 0) {
        int valueSize = characteristic->valueSize();
        m_longWriteValue = (uint8_t*)realloc(m_longWriteValue, valueSize);
        m_longWriteValueLength = 0;
        m_longWriteHandle = h;
        memset(m_longWriteValue, 0x00, valueSize);
    } else if (m_longWriteHandle != h) {
        sendError(handle, ATT_OP_PREP_WRITE_REQ, h, ATT_ECODE_UNLIKELY);
        return;
    }
    uint8_t  valueLength = length - sizeof(PrepWriteReq);
    uint8_t* value = &data[sizeof(PrepWriteReq)];
    if ((o != m_longWriteValueLength) || ((o + valueLength) > (uint16_t)characteristic->valueSize())) {
        sendError(handle, ATT_OP_PREP_WRITE_REQ, h, ATT_ECODE_INVALID_OFFSET);
        return;
    }
    memcpy(m_longWriteValue + o, value, valueLength);
    m_longWriteValueLength += valueLength;
    uint8_t  response[m_mtu];
    uint16_t responseLength;
    response[0] = ATT_OP_PREP_WRITE_RESP;
    memcpy(&response[1], data, length);
    responseLength = length + 1;
    g_hci.sendAclPacket(handle, ATT_CID, responseLength, response);
}

void ATTClass::execWriteReq(uint16_t handle, uint8_t length, uint8_t data[])
{
    if (length != sizeof(uint8_t)) {
        sendError(handle, ATT_OP_EXEC_WRITE_REQ, 0x0000, ATT_ECODE_INVALID_PDU);
        return;
    }
    uint8_t flag = data[0];
    if (m_longWriteHandle && (flag & 0x01)) {
        LocalCharacteristic* characteristic = (LocalCharacteristic*)g_gatt.attribute(m_longWriteHandle - 1);
        if (handle == m_connectionHandle) {
            characteristic->writeValue(BLEDevice(handle, m_peerAddress), m_longWriteValue, m_longWriteValueLength);
        }
    }
    m_longWriteHandle = 0x0000;
    m_longWriteValueLength = 0;
    uint8_t  response[m_mtu];
    uint16_t responseLength;
    response[0] = ATT_OP_EXEC_WRITE_RESP;
    responseLength = 1;
    g_hci.sendAclPacket(handle, ATT_CID, responseLength, response);
}

void ATTClass::handleCnf(uint16_t, uint8_t, uint8_t[])
{
    m_cnf = true;
}

void ATTClass::sendError(uint16_t handle, uint8_t opcode, uint16_t errHandle, uint8_t code)
{
    struct __attribute__ ((packed)) {
        uint8_t  op;
        uint8_t  opcode;
        uint16_t handle;
        uint8_t  code;
    } attError = { ATT_OP_ERROR, opcode, errHandle, code };

    g_hci.sendAclPacket(handle, ATT_CID, sizeof(attError), &attError);
}

void ATTClass::setEventHandler(BLEDeviceEvent event, BLEDeviceEventHandler eventHandler)
{
    if (event < (sizeof(m_eventHandlers) / (sizeof(m_eventHandlers[0])))) {
        m_eventHandlers[event] = eventHandler;
    }
}

ATTClass g_att;


