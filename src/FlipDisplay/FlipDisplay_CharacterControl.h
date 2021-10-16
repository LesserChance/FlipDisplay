#ifndef FLIPDISPLAY_CHARACTERCONTROL_H
#define FLIPDISPLAY_CHARACTERCONTROL_H

#include "FlipDisplay_Config.h"
#include "FlipDisplay_LoopDetection.h"
#include "FlipDisplay_Registers.h"

/**
 * Perform the initial character control setup
 */
void setupCharacterControl();

/**
 * Set the display to a new value
 */
void setDisplayString(String newString);

/**
 * move the passed motor to a new character
 */
void setCharacter(int board, int motor, char character);

/**
 * Save the current character of the motor
 */
void storeCurrentCharacter(int board, int motor, int character);

/**
 * return the number of steps it takes to move this motor one character
 */
int getStepsPerCharacter(int board, int motor);

/**
 * add the offset amount to the current remaining steps
 */
void addOffsetSteps(int board, int motor);

/**
 * get the number of steps the motor needs to move to reach the new character
 */
int getCharDistance(int board, int motor, char target);

/**
 * step any motors that are active moving
 */
void stepMovingMotors();

bool allMotorsArePaused();

#endif
