#ifndef Config_h
#define Config_h

#include <Arduino.h>

#define DEBUG 1
#define DEBUG_LOOP 0
#define SERVER_ENABLED 1

#define loopPin1 2
#define loopPin2 5
#define loopPin3 19
#define loopPin4 22
#define loopPin5 35
#define loopPin6 25

#define stepPin1 4
#define stepPin2 18
#define stepPin3 21
#define stepPin4 23
#define stepPin5 32
#define stepPin6 26

#define enablePin 13

// #define TEST_PIN_ONE 5          // trigger lerp (yellow)
// #define TEST_PIN_TWO 2         // trigger home (green)

#define TOTAL_CHARACTERS 6                 // the total number of characters we're able to control
#define START_CHARACTER 0                  // which index is the first character (allows you to test just characters at the end)
#define CHARACTER_COUNT 6                  // how many characters will the animations actually control (out of total)

#define MIN_STEP_DURATION 2000             // time between steps when moving as fast as possible
#define AVG_STEP_DURATION 4600             // time between steps for an average running speed
#define HOMING_STEP_DURATION 6000          // time between steps when homing

#define LOOP_REVERSE_OFFSET 1              // for a loop detected while running, this offset adjusts for the fact that its moving fast and oversteps

#define DEBOUNCE_DURATION 50000            // how long to wait (microseconds) to debounce the loop button
#define STEPS_PER_REVOLUTION 400           // total numbers of steps to make a full rotation
#define POSSIBLE_CHARACTER_VALUES 40       // total number of characters that can be displayed
#define STEPS_PER_CHARACTER 10             // how many steps to step a single character
#define MIN_STEPS_PER_LERP 200             // if a lerp is below this number of steps, itll do a full loop
#define PAUSE_ENABLED_DURATION 1000000 * 5 // how long to wait (microseconds) before disabling the motor entirely when paused

#define SCROLLING_ENABLED 1                // should the display scroll for strings that are too large?
#define WAIT_TO_SCROLL 1000000 * 1         // how long should we pause until we scroll to the next set of characters
#define CHARS_TO_SCROLL 3                  // how many characters should scroll at a time

const int STEP_PIN[TOTAL_CHARACTERS] = {stepPin1, stepPin2, stepPin3, stepPin4, stepPin5, stepPin6};
const int LOOP_PIN[TOTAL_CHARACTERS] = {loopPin1, loopPin2, loopPin3, loopPin4, loopPin5, loopPin6};
const int CHARACTER_OFFSET[TOTAL_CHARACTERS] = {45,38,37,54,34,34};

#endif