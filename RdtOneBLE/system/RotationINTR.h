/*
 * RotationINTR.h
 *
 * Created: 2019-04-06 11:34:52
 *  Author: kong
 */ 


#ifndef ROTATIONINTR_H_
#define ROTATIONINTR_H_

#define WHEEL_SIZE 2 // 2m

class RotationINTR
{
public:
    RotationINTR();
    ~RotationINTR();
    int  begin();
    unsigned int getVelocity();
    void isr();
private:
    unsigned long m_rotation;
    unsigned long m_msec;
    unsigned int  m_meter_per_sec;
};

extern RotationINTR x_rotation;

#endif /* ROTATIONINTR_H_ */