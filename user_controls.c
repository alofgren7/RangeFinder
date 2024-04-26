/**************************************************************************/ /**
                                                                              *
                                                                              * @file user_controls.c
                                                                              *
                                                                              * @author (STUDENTS -- TYPE YOUR NAMES HERE)
                                                                              * @author (STUDENTS -- TYPE YOUR NAMES HERE)
                                                                              *
                                                                              * @brief Code to get inputs from the user.
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
#include <string.h>
#include <stdlib.h>
#include "user_controls.h"
#include "shared_variables.h"
#define DEBOUNCE_TIME 150u
enum State curr_state = NORMAL;
enum State prev_state;
static char *stateStrings[] = {"NORMAL", "SINGLE PULSE", "THRESHOLD ADJUSTMENT",
                               "CONTINUOUS TONE"};
volatile cowpi_ioport_t *ioports = (cowpi_ioport_t *)(0x23); // an array of I/O
ports static bool left_switch = false;
static bool right_switch = false;
static bool left_button_pressed = false;
bool ping_registered;
static bool button_held_down = false;
bool threshold_received = false;
volatile uint16_t threshold_range;
volatile unsigned int normal_counter;
bool left_switch_is_in_left_position(void);
bool left_switch_is_in_right_position(void);
bool right_switch_is_in_left_position(void);
bool right_switch_is_in_right_position(void);
bool left_button_is_pressed(void);
void set_state(void);
void poll_switches(void);
void poll_left_button(void);
void handle_threshold(void);
char get_keypress(void);
void initialize_controls(void)
{
    ping_registered = false;
}
void manage_controls(void)
{
    poll_switches();
    poll_left_button();
    if (curr_state == THRESHOLD_ADJUSTMENT)
    {
        cowpi_deluminate_right_led();
        if (!threshold_received)
        {
            handle_threshold();
        }
    }
}
void handle_threshold(void)
{
    unsigned int range = 0;
    volatile uint16_t number_index = 0;
    volatile char number[] = {'\0', '\0', '\0', '\0'};
    fprintf(display, "\nEnter Range: \n");
    while (number_index <= 3)
    {
        char key = get_keypress();
        if (key >= '0' && key <= '9' && number_index < 3)
        {
            number[number_index] = key;
            number_index++;
            fprintf(display, "\n%s cm\n", number);
        }
        else if (key == '#' && number_index > 0)
        {
            range = atoi(number);
            number_index = 4;
        }
    }
    bool valid;
    if (range > 400 || range < 50)
    {
        valid = false;
    }
    else if (range <= 400 || range >= 50)
    {
        valid = true;
    }
    if (valid)
    {
        fprintf(display, "\nThreshold %dcm\n", range);
        threshold_range = range;
        threshold_received = true;
    }
    else if (!valid)
    {
        fprintf(display, "\nTry again\n");
        while (normal_counter < 20000)
        {
        }
    }
}
char get_keypress(void)
{
    static unsigned long last_keypress = 0uL;
    unsigned long now = millis();
    char key = 'n';
    if (now - last_keypress > DEBOUNCE_TIME)
    {
        last_keypress = now;
        key = cowpi_get_keypress();
    }
    return key;
}
void poll_left_button(void)
{
    bool is_button_pressed = left_button_is_pressed();
    if (is_button_pressed != button_held_down && curr_state == SINGLE_PULSE)
    {
        if (is_button_pressed)
        {
            ping_registered = true;
            // printf("\nPing\n");
        }
        button_held_down = is_button_pressed;
    }
}
void poll_switches(void)
{
    bool change = false;
    if (left_switch_is_in_right_position() != left_switch)
    {
        left_switch = !left_switch;
        change = true;
    }
    else if (right_switch_is_in_right_position() != right_switch)
    {
        right_switch = !right_switch;
        change = true;
    }
    // do some things if there's been a change
    if (change)
    {
        set_state();
        printf("\nSTATE");
        printf("\n%s\n", stateStrings[curr_state]);
    }
}
void set_state(void)
{
    if (left_switch && right_switch)
    {
        curr_state = THRESHOLD_ADJUSTMENT;
    }
    else if (left_switch && !right_switch)
    {
        curr_state = SINGLE_PULSE;
    }
    else if (!left_switch && !right_switch)
    {
        curr_state = NORMAL;
    }
    else if (!left_switch && right_switch)
    {
        curr_state = CONTINUOUS_TONE;
    }
}
bool left_switch_is_in_left_position(void)
{
    return !left_switch_is_in_right_position();
}
bool left_switch_is_in_right_position(void)
{
    bool switch_in_position = cowpi_left_switch_is_in_right_position();
    return cowpi_debounce_byte(switch_in_position, LEFT_SWITCH_RIGHT);
}
bool right_switch_is_in_left_position(void)
{
    return !right_switch_is_in_right_position();
}
bool right_switch_is_in_right_position(void)
{
    bool switch_in_position = cowpi_right_switch_is_in_right_position();
    return cowpi_debounce_byte(switch_in_position, RIGHT_SWITCH_RIGHT);
}
bool left_button_is_pressed(void)
{
    bool button_is_pressed = cowpi_left_button_is_pressed();
    return cowpi_debounce_byte(button_is_pressed, LEFT_BUTTON_DOWN);
}