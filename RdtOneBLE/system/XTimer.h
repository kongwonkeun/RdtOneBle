/*
 * XTimer.h
 *
 * Created: 2019-03-29 13:21:17
 *  Author: kong
 */

#ifndef XTIMER_H_
#define XTIMER_H_

class XTimer
{
public:
    XTimer();
    ~XTimer();
    void begin(int id);
    void end(int id);
    unsigned long millisec();
    unsigned long sec();
    void isr();
private:
    unsigned long m_millisec;
    unsigned long m_sec;
    int m_id;
};

extern XTimer g_timer0;

#endif /* XTIMER_H_ */