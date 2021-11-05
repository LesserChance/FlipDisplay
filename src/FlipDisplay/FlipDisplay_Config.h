#ifndef FLIPDISPLAY_CONFIG_H
#define FLIPDISPLAY_CONFIG_H

#include "Arduino.h"

// available characters
const String CHARACTERS = " ABCDEFGHIJKLMNOPQRSTUVWXYZ23456789()'.-";

// how many boards to control
const int BOARD_COUNT = 1;

// how many motors to control per board
const int MOTOR_COUNT = 4;

// how many valid characters on each digit
const int CHAR_COUNT = 40; 

// how many phases for the motor steps
const int PHASE_COUNT = 4; 

// how many microseconds to delay after writing to the registers during normal operation
const int DELAY_AMT = 2000;

// how many milliseconds to wait to debounce the start trigger button
const int DEBOUNCE_WAIT_TIME = 10;

// how many steps does it take to do a full loop
const int STEPS_PER_LOOP = 4096;

// how many steps should be taken to get to the next character
const int STEPS_PER_CHAR = 102;

// since we cant get a perfect number of steps per character, how many extra steps need to happen to catch up per loop
const int CATCH_UP_STEPS = STEPS_PER_LOOP - (STEPS_PER_CHAR * CHAR_COUNT);

// some motors dont start at the blank tile perfectly, how many more steps should they step to hit it perfectly
const int CHARACTER_OFFSET[BOARD_COUNT][MOTOR_COUNT] = {{38, 138, 156, 58}};

// how many loops should be allowed before resetting the zero position - higher makes it less likely for collisions
const int LOOP_RESET_THRESHOLD = 2;

// how long to stay on a word
const int WORD_PAUSE_TIME = 30000;

// how many hours to offset GMT Timezone
const int GMT_OFFSET = -4;

// when updating board count these need to be updated
const int MOTOR_LOOP_PIN[BOARD_COUNT] = {23};  // pull low with a 220 resistor on the main board {23, 4}
const int MOTOR_ENABLE_PIN[BOARD_COUNT] = {21}; // pin 13 on the 74hc595, yellow, OE go LOW to enable {21, 2}

// list of valid instructions to be queued
const int INSTRUCTION_SET_DISPLAY = 0;
const int INSTRUCTION_RESET_ALL = 1;

#endif
