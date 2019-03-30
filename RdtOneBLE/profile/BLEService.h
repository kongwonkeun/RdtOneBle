/*
 * BLEService.h
 *
 * Created: 2019-03-27 11:43:30
 *  Author: kong
 */


#ifndef BLESERVICE_H_
#define BLESERVICE_H_

class LocalService;
class LocalCharacteristic;
class BLECharacteristic;

class BLEService
{
public:
    BLEService();
    BLEService(const char* uuid);
    virtual ~BLEService();
    const char* uuid();
    void addCharacteristic(BLECharacteristic& characteristic);
    operator bool();

protected:
    friend class GATTClass;
    BLEService(LocalService* local);
    LocalService* local();
    void addCharacteristic(LocalCharacteristic* characteristic);

private:
    LocalService* m_local;
};


#endif /* BLESERVICE_H_ */