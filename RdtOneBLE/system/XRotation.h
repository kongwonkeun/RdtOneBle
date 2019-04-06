/*
 * XRotation.h
 *
 * Created: 2019-04-04 17:38:55
 *  Author: kong
 */ 

#ifndef XROTATION_H_
#define XROTATION_H_

class XRotation
{
public:
    XRotation();
    ~XRotation();
    int  begin();
    void isr();
private:
    unsigned long m_rotation;
};

extern XRotation g_rotation;

#endif /* XROTATION_H_ */