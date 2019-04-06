/*
 * XDistance.cpp
 *
 * Created: 2019-04-03 14:23:01
 *  Author: kong
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "system/XUtil.h"
#include "system/XConsole.h"
#include "system/XDistance.h"

ISR(ADC_vect) {
    g_distance.isr();
}

XDistance::XDistance() : m_channel(0), m_centimeter(0), m_maxValue(1023), m_maxMilliVolt(5000)
{
    //
}

XDistance::~XDistance()
{
    //
}

void XDistance::begin()
{
    // prescaler(128) --> 16MHz/128 = 125KHz
    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
    sbi(ADMUX, REFS0); // ref: AVCC, right adjust, ADC0
    sbi(ADCSRA, ADEN); // adc enable
    //sbi(ADCSRA, ADIE); // adc interrupt enable
    //sbi(ADCSRA, ADSC); // adc start
}

void XDistance::end()
{
    //
}

unsigned int XDistance::getDistance()
{
    unsigned int millivolt = calculateMilliVolt(readAdc0());
    return calculateCentiMeter(millivolt);
}

unsigned int XDistance::readAdc0()
{
    outb(ADMUX, inb(ADMUX) & 0xf0);
    outb(ADMUX, inb(ADMUX) | m_channel);
    sbi(ADCSRA, ADSC); // start
    while (bit_is_set(ADCSRA, ADSC)); // wait until the complete of conversion
    return inb(ADCW);
}

void XDistance::isr()
{
    m_value = inb(ADCW);
    m_centimeter = calculateCentiMeter(calculateMilliVolt(m_value));
    sbi(ADCSRA, ADSC); // start conversion
}

unsigned int XDistance::calculateMilliVolt(unsigned int value)
{
    return map(value, 0, m_maxValue, 0, m_maxMilliVolt);
    //return (m_maxMilliVolt * value) / m_maxValue;
}

unsigned int XDistance::calculateCentiMeter(unsigned int millivolt)
{
    unsigned long centimeter = 1000;
    //return (centimeter * 27.61) / (millivolt)
    return (centimeter * 23) / (millivolt + 50);
}

XDistance g_distance;

/* EOF */