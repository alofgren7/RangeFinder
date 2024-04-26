/**************************************************************************/ /**
                                                                              *
                                                                              * @file alarm.c
                                                                              *
                                                                              * @author Andrew Lofgren
                                                                              * @author (STUDENTS -- TYPE YOUR NAMES HERE)
                                                                              *
                                                                              * @brief Code to manage the piezodisc and LEDs.
                                                                              *
                                                                              ******************************************************************************/
/*
 * RangeFinder GroupLab assignment and starter code (c) 2023 Christopher A. Bohn
 * RangeFinder solution (c) the above-named students
 */
#include <CowPi.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "alarm.h"
#include "shared_variables.h"
const unsigned int on_period = 500; // a made-up number, probably not the
value you want to use volatile unsigned int total_period = 50000;
volatile cowpi_timer16bit_t *timer;
bool alarm_on = false;
enum State curr_state;
volatile unsigned int isr_counter = 0;
bool alarm_request;
volatile uint16_t threshold_range;
unsigned long object_distance;
volatile uint16_t normal_counter;
bool object_detected;
unsigned long rate_of_approach;
void find_total_period(void);
void initialize_alarm(void)
{
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2A |= (1 << WGM21);
    TCCR2B |= (1 << CS21);
    OCR2A = 199;
    TIMSK2 |= (1 << OCIE2A);
    sei();
}
void manage_alarm(void)
{
    if (alarm_request)
    {
        alarm_on = true;
        isr_counter = 0;
        alarm_request = false;
    }
}
void find_total_period(void)
{
    if (object_distance >= 250)
    {
        total_period = 25000;
    }
    else if (object_distance >= 200)
    {
        total_period = 20000;
    }
    else if (object_distance >= 150)
    {
        total_period = 15000;
    }
    else if (object_distance >= 100)
    {
        total_period = 10000;
    }
    else if (object_distance >= 50)
    {
        total_period = 7500;
    }
    else if (object_distance >= 25)
    {
        total_period = 5000;
    }
    else if (object_distance >= 10)
    {
        total_period = 2500;
    }
    else if (object_distance >= 0)
    {
        total_period = 1250;
    }
}
ISR(TIMER2_COMPA_vect)
{
    if (curr_state == CONTINUOUS_TONE)
    {
        digitalWrite(13, !digitalRead(13));
    }
    if (alarm_on)
    {
        if (isr_counter < on_period)
        {
            if (object_distance < threshold_range)
            {
                digitalWrite(13, !digitalRead(13));
            }
            cowpi_illuminate_left_led();
            cowpi_illuminate_right_led();
        }
        else
        {
            cowpi_deluminate_left_led();
            cowpi_deluminate_right_led();
            alarm_on = false;
            isr_counter = 0;
        }
    }
    if (curr_state == THRESHOLD_ADJUSTMENT)
    {
        normal_counter++;
        if (normal_counter == 20001)
        {
            normal_counter = 0;
        }
    }
    if (curr_state == NORMAL)
    {
        find_total_period();
        if (isr_counter < on_period)
        {
            cowpi_illuminate_left_led();
            cowpi_illuminate_right_led();
            if (object_detected && rate_of_approach != 0 && object_distance < threshold_range)
            {
                digitalWrite(13, !digitalRead(13));
            }
        }
        else if (isr_counter > total_period - on_period)
        {
            cowpi_deluminate_left_led();
            cowpi_deluminate_right_led();
            isr_counter = 0;
        }
        isr_counter++;
        normal_counter++;
        if (normal_counter == 10001)
        {
            normal_counter = 0;
        }
    }
}