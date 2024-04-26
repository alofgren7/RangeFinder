/**************************************************************************/ /**
                                                                              *
                                                                              * @file sensor.c
                                                                              *
                                                                              * @author Andrew Lofgren
                                                                              * @author (STUDENTS -- TYPE YOUR NAMES HERE)
                                                                              *
                                                                              * @brief Code to manage the distance sensor.
                                                                              *
                                                                              ******************************************************************************/
/*
 * RangeFinder GroupLab assignment and starter code (c) 2023 Christopher A. Bohn
 * RangeFinder solution (c) the above-named students
 */
#include <CowPi.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "sensor.h"
#include "shared_variables.h"
bool object_detected;
unsigned long object_distance;
unsigned long last_object_distance = 0;
unsigned long rate_of_approach;
bool ping_registered;
bool rising = false;
enum MachineState machine_state = READY;
volatile uint16_t timer1_counter = 0;
enum State curr_state;
static char *machineStates[] = {"Ready", "Active listening", "Active detected",
                                "Quiescent"};
bool distance_calculated = false;
bool alarm_request = false;
volatile uint16_t threshold_range = 400;
volatile unsigned int normal_counter;
volatile uint16_t detections = 0;
volatile uint16_t threshold_range;
bool threshold_received;
void handle_sensor(void);
void calculate_distance(void);
void single_pulse(void);
void calculate_speed(void);
void single_pulse(void)
{
    digitalWrite(2, HIGH);
    ping_registered = false;
    delayMicroseconds(10);
    digitalWrite(2, LOW);
    // printf("\n%s\n", machineStates[machine_state]);
    if (machine_state == QUIESCENT)
    {
        calculate_distance();
    }
}
void handle_sensor(void)
{
    if (digitalRead(3))
    {
        TCNT1 = 0;
        machine_state = ACTIVE_LISTENING;
        // printf("\nrising\n");
    }
    else
    {
        if (!digitalRead(3) && machine_state == ACTIVE_LISTENING)
        {
            timer1_counter = TCNT1;
            // printf("\n%d\n", timer1_counter);
            machine_state = ACTIVE_DETECTED;
            object_detected = true;
            // printf("\nfalling\n");
        }
    }
}
void initialize_sensor(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 32768;
    TIMSK1 |= (1 << TOIE1);
    TCCR1B |= (1 << CS11);
    sei();
    cowpi_register_pin_ISR((1L << 3), handle_sensor);
}
void manage_sensor(void)
{
    if (ping_registered)
    {
        single_pulse();
    }
    else if (curr_state == NORMAL)
    {
        threshold_received = false;
        // printf("\n%s\n", machineStates[machine_state]);
        if (machine_state == READY)
        {
            digitalWrite(2, HIGH);
            delayMicroseconds(10);
            digitalWrite(2, LOW);
        }
        if (machine_state == QUIESCENT)
        {
            calculate_speed();
        }
    }
}
void calculate_speed()
{
    if (object_detected)
    {
        if (normal_counter == 10000)
        {
            last_object_distance = object_distance;
            object_distance = ((timer1_counter * 18025UL) >> 21);
            if (detections > 1)
            {
                rate_of_approach = object_distance - last_object_distance;
                fprintf(display, "\nSpeed (cm/s): %ld\n", ((long)last_object_distance - (long)object_distance));
            }
            detections++;
        }
        distance_calculated = true;
    }
    else if (!object_detected)
    {
        object_detected = false;
        fprintf(display, "\nNo object\n");
    }
}
void calculate_distance(void)
{
    if (object_detected)
    {
        // printf("\nObject Detected\n");
        alarm_request = true;
        object_distance = ((timer1_counter * 18025UL) >> 21);
        fprintf(display, "\n%lu cm\n", object_distance);
        object_detected = false;
    }
    else if (!object_detected)
    {
        fprintf(display, "\nNo object\n");
        object_detected = false;
    }
    distance_calculated = true;
}
ISR(TIMER1_OVF_vect)
{
    if (machine_state == ACTIVE_LISTENING)
    {
        machine_state = QUIESCENT;
        // printf("\nNo object detected\n");
    }
    else if (machine_state == ACTIVE_DETECTED)
    {
        // printf("\nObject detected\n");
        machine_state = QUIESCENT;
    }
    else if (machine_state == QUIESCENT && distance_calculated)
    {
        machine_state = READY;
        distance_calculated = false;
        // printf("\nReady\n");
    }
    // printf("\nINTERRUPT\n");
    TCNT1 = 0;
}