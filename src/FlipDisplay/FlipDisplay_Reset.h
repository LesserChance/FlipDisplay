#ifndef FLIPDISPLAY_RESET_H
#define FLIPDISPLAY_RESET_H

#include "Arduino.h"
#include "FlipDisplay_CharacterControl.h"
#include "FlipDisplay_Config.h"
#include "FlipDisplay_LoopDetection.h"
#include "FlipDisplay_Queue.h"
#include "FlipDisplay_Registers.h"

/**
 * add a parallel reset into the instruction queue
 */
void queueReset(int delayBeforeReset);

/**
 * reset all motors to their starting position
 */
void resetAll();

/**
 * first step, move past the button one by one
 */
void reset_getPastButtons();

/**
 * second step, move to the button all at once
 */
void reset_moveToButtons();

/**
 * third step: advance more to match the starting offset
 */
void reset_moveToStartPosition();

#endif
