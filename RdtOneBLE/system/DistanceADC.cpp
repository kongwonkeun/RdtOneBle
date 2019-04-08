/*
 * DistanceADC.cpp
 *
 * Created: 2019-04-06 11:52:17
 *  Author: kong
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "system/Util.h"
#include "system/ConsoleUART.h"
#include "system/BluetoothUART.h"
#include "system/DistanceADC.h"
#include "main.h"

ISR(ADC_vect) {
    x_distance.isr();
}

DistanceADC::DistanceADC() : m_channel(0), m_centimeter(0), m_maxValue(1023), m_maxMilliVolt(5000)
{
}

DistanceADC::~DistanceADC()
{
}

void DistanceADC::begin()
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

void DistanceADC::end()
{
}

unsigned int DistanceADC::getDistance()
{
    unsigned int millivolt = calculateMilliVolt(readAdc0());
    /*
    char str[10];
    wordToString(millivolt, str);
    x_console.write(str);
    x_console.write("\n");
    */
    return calculateCentiMeter(millivolt);
}

unsigned int DistanceADC::readAdc0()
{
    outb(ADMUX, inb(ADMUX) & 0xf0);
    outb(ADMUX, inb(ADMUX) | m_channel);
    sbi(ADCSRA, ADSC); // start
    while (bit_is_set(ADCSRA, ADSC)); // wait until the complete of conversion
    unsigned int value = inb(ADCW);
    /*
    char str[10];
    wordToString(value, str);
    x_console.write(str);
    x_console.write("\n");
    */
    return value;
}

void DistanceADC::isr()
{
    m_value = inb(ADCW);
    m_centimeter = calculateCentiMeter(calculateMilliVolt(m_value));
    if (x_manual_mode) return;
    if (x_showup) x_console.write("D");
    //
    sbi(ADCSRA, ADSC); // start conversion
}

unsigned int DistanceADC::calculateMilliVolt(unsigned int value)
{
    return map((unsigned long)value, 0, m_maxValue, 0, m_maxMilliVolt);
}

unsigned int DistanceADC::calculateCentiMeter(unsigned int millivolt)
{
    unsigned long centimeter = 1000;
    //return (centimeter * 27.61) / (millivolt)
    return (centimeter * 23) / (millivolt + 50);
}

DistanceADC x_distance;

/* EOF */