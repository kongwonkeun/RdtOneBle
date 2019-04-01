/*
 * BLEProperty.h
 *
 * Created: 2019-03-27 13:31:07
 *  Author: kong
 */

#ifndef BLEPROPERTY_H_
#define BLEPROPERTY_H_

enum BLEProperty
{
    BPP_BROADCAST = 0x01,
    BPP_READ = 0x02,
    BPP_WRITE_WO_RESPONSE = 0x04,
    BPP_WRITE = 0x08,
    BPP_NOTIPY = 0x10,
    BPP_INDICATE = 0x20
};

#endif /* BLEPROPERTY_H_ */