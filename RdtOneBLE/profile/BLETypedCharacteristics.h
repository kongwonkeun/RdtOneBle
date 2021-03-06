﻿/*
 * BLETypedCharacteristics.h
 *
 * Created: 2019-03-27 14:11:42
 *  Author: kong
 */

#ifndef BLETYPEDCHARACTERISTICS_H_
#define BLETYPEDCHARACTERISTICS_H_

#include "system/Util.h"
#include "profile/BLETypedCharacteristic.h"

class BLEBoolCharacteristic : public BLETypedCharacteristic<bool>
{
public:
    BLEBoolCharacteristic(const char* uuid, unsigned char properties);
};

class BLEBooleanCharacteristic : public BLETypedCharacteristic<boolean>
{
public:
    BLEBooleanCharacteristic(const char* uuid, unsigned char properties);
};

class BLECharCharacteristic : public BLETypedCharacteristic<char>
{
public:
    BLECharCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedCharCharacteristic : public BLETypedCharacteristic<unsigned char>
{
public:
    BLEUnsignedCharCharacteristic(const char* uuid, unsigned char properties);
};

class BLEByteCharacteristic : public BLETypedCharacteristic<byte>
{
public:
    BLEByteCharacteristic(const char* uuid, unsigned char properties);
};

class BLEShortCharacteristic : public BLETypedCharacteristic<short>
{
public:
    BLEShortCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedShortCharacteristic : public BLETypedCharacteristic<unsigned short>
{
public:
    BLEUnsignedShortCharacteristic(const char* uuid, unsigned char properties);
};

class BLEWordCharacteristic : public BLETypedCharacteristic<word>
{
public:
    BLEWordCharacteristic(const char* uuid, unsigned char properties);
};

class BLEIntCharacteristic : public BLETypedCharacteristic<int>
{
public:
    BLEIntCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedIntCharacteristic : public BLETypedCharacteristic<unsigned int>
{
public:
    BLEUnsignedIntCharacteristic(const char* uuid, unsigned char properties);
};

class BLELongCharacteristic : public BLETypedCharacteristic<long>
{
public:
    BLELongCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedLongCharacteristic : public BLETypedCharacteristic<unsigned long>
{
public:
    BLEUnsignedLongCharacteristic(const char* uuid, unsigned char properties);
};

class BLEFloatCharacteristic : public BLETypedCharacteristic<float>
{
public:
    BLEFloatCharacteristic(const char* uuid, unsigned char properties);
};

class BLEDoubleCharacteristic : public BLETypedCharacteristic<double>
{
public:
    BLEDoubleCharacteristic(const char* uuid, unsigned char properties);
};

#endif /* BLETYPEDCHARACTERISTICS_H_ */