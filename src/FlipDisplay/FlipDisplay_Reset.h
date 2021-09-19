#ifndef FLIPDISPLAY_RESET_H
#define FLIPDISPLAY_RESET_H

#include "Arduino.h"
#include "FlipDisplay_CharacterControl.h"
#include "FlipDisplay_Config.h"
#include "FlipDisplay_LoopDetection.h"
#include "FlipDisplay_Registers.h"

/**
 * reset all motors to their starting position
 */
void resetAll();

/**
 * reset a single motors to its starting position
 */
void resetMotor(int board, int motor);

/**
 * Send the specified motor to its starting position
 */
void resetMotorPosition(int board, int motor);

#endif