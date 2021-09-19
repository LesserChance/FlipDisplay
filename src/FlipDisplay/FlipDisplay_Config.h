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

// how many milliseconds to delay after writing to the registers during normal operation
const int DELAY_AMT = 2;

// how many milliseconds to delay after writing to the registers during a reset operation
const int DELAY_AMT_RESET = 2;

// how many milliseconds to wait to debounce the start trigger button
const int DEBOUNCE_WAIT_TIME = 10;

#endif