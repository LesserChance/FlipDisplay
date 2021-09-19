#ifndef FLIPDISPLAY_CHARACTERCONTROL_H
#define FLIPDISPLAY_CHARACTERCONTROL_H

#include "FlipDisplay_Config.h"
#include "FlipDisplay_LoopDetection.h"
#include "FlipDisplay_Registers.h"

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
 * get the offset amount (how many steps the motor needs to move to be perfectly aligned at 0)
 */
int getCharacterOffset(int board, int motor);

/**
 * get the number of steps the motor needs to move to reach the new character
 */
int getCharDistance(int board, int motor, char target);

/**
 * step any motors that are active moving
 */
void stepMovingMotors();

#endif