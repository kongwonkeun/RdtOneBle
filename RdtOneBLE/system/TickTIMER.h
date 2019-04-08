/*
 * TickTIMER.h
 *
 * Created: 2019-04-06 11:26:41
 *  Author: kong
 */ 

#ifndef TICKTIMER_H_
#define TICKTIMER_H_

class TickTIMER
{
public:
    TickTIMER();
    ~TickTIMER();
    void begin(unsigned int interval);
    void end();
    unsigned long getMillisec();
    unsigned long getSec();
    void incInterval();
    void decInterval();
    void isr();
private:
    unsigned long m_millisec;
    unsigned long m_sec;
    unsigned int  m_interval;
};

extern TickTIMER x_tick;

#endif /* TICKTIMER_H_ */