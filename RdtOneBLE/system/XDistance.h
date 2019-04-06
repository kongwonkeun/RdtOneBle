/*
 * XDistance.h
 *
 * Created: 2019-04-03 14:11:21
 *  Author: kong
 */ 

#ifndef XDISTANCE_H_
#define XDISTANCE_H_

class XDistance
{
public:
    XDistance();
    ~XDistance();
    void begin();
    void end();
    unsigned int  getDistance();
    void isr();
private:
    unsigned int  readAdc0();
    unsigned int  calculateMilliVolt(unsigned int value);
    unsigned int  calculateCentiMeter(unsigned int millivolt);
    unsigned char m_channel;
    unsigned int  m_centimeter;
    unsigned int  m_maxValue;
    unsigned int  m_maxMilliVolt;
    unsigned int  m_value;
};

extern XDistance g_distance;

#endif /* XDISTANCE_H_ */