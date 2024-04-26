/**************************************************************************/ /**
                                                                              *
                                                                              * @file shared_variables.h
                                                                              *
                                                                              * @author (STUDENTS -- TYPE YOUR NAMES HERE)
                                                                              * @author (STUDENTS -- TYPE YOUR NAMES HERE)
                                                                              *
                                                                              * @brief Types and variables that need to be shared among the .c files.
                                                                              *
                                                                              ******************************************************************************/
/*
 * RangeFinder GroupLab assignment and starter code (c) 2023 Christopher A. Bohn
 * RangeFinder solution (c) the above-named students
 */
#include <CowPi.h>
#include <stdbool.h>
#include <stdint.h>
#ifndef SHARED_VARIABLES_H
#define SHARED_VARIABLES_H
#ifdef __cplusplus
extern "C"
{
#endif
    /*** DO NOT PLACE ANY CODE ABOVE THIS LINE! ***/
    /* IF YOU NEED TO DECLARE ANY ENUMS OR STRUCTS THAT
     ARE USED BY MORE THAN ONE .c FILE, PLACE THEM HERE */
    extern enum State {
        NORMAL,
        SINGLE_PULSE,
        THRESHOLD_ADJUSTMENT,
        CONTINUOUS_TONE
    };
    extern enum MachineState {
        READY,
        ACTIVE_LISTENING,
        ACTIVE_DETECTED,
        QUIESCENT
    };
    extern bool object_detected;
    extern enum MachineState machine_state;
    extern enum State curr_state;
    extern enum State prev_state;
    extern bool ping_registered;
    extern bool object_detected;
    extern unsigned long object_distance;
    extern unsigned long rate_of_approach;
    extern bool alarm_request;
    extern volatile uint16_t threshold_range;
    extern unsigned long object_distance;
    extern volatile unsigned int normal_counter;
    extern bool threshold_received;
    /* IF YOU NEED TO CREATE ANY GLOBAL VARIABLES
     THAT MUST BE SHARED BETWEEN .c FILES, THEN:
     - DECLARE THEM IN *ONE AND ONLY ONE* .c FILE AS A volatile VARIABLE
     example: volatile unsigned long foo;
     - DECLARE THEM HERE AS AN extern volatile VARIABLE
     example: extern volatile unsigned long foo;
    */
    extern FILE *display;
/*** DO NOT PLACE ANY CODE BELOW THIS LINE! ***/
#ifdef __cplusplus
} // extern "C"
#endif
#endif // SHARED_VARIABLES_H