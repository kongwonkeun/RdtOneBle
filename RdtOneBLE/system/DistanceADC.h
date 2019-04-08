/*
 * DistanceADC.h
 *
 * Created: 2019-04-06 11:50:31
 *  Author: kong
 */

#ifndef DISTANCEADC_H_
#define DISTANCEADC_H_

class DistanceADC
{
public:
    DistanceADC();
    ~DistanceADC();
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
    unsigned long m_maxValue;
    unsigned long m_maxMilliVolt;
    unsigned int  m_value;
};

extern DistanceADC x_distance;

#endif /* DISTANCEADC_H_ */