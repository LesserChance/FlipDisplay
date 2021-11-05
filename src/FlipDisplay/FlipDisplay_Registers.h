#ifndef FLIPDISPLAY_REGISTERS_H
#define FLIPDISPLAY_REGISTERS_H

#include "Arduino.h"
#include "FlipDisplay_Config.h"
#include "FlipDisplay_Util.h"

/**
 * Initialize the shift register pins and data
 */
void setupRegisters();

/**
 * return the pin to use for the interrupt tirgger
 */
int getMotorStartPin(int board);

/**
 * put a motor into a paused state
 */
void pauseMotor(int board, int motor);

/**
 * step a motor a single time
 */
void stepMotor(int board, int motor);

/**
 * Read the motor start pin, and return true if its triggered
 */
int isButtonTriggered(int board);

/**
 * Update the button register to read from the passed in board/motor
 */
void updateButtonRegister(int board, int motor);

/**
 * reset the button register to not read from any motors
 */
void resetButtonRegister(int board);

/**
 * Set the button register to a new value
 */
void setButtonRegister(int board, byte data);

/**
 * given the current button and motors states, update the shift register data
 */
void updateMotorRegister(int board, int motor);

/**
 * output the current data to the shift registers, using the default delay amount
 */
void writeRegisters();

/**
 * output the current data to the shift registers
 */
void writeRegistersWithDelay(int delayAmt);

void disableBoard(int board);
void enableBoard(int board);

#endif
