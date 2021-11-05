#include "FlipDisplay_Reset.h"

void queueReset(int delayBeforeReset) {
  String ellipse = "";
  for (int i = 0; i < BOARD_COUNT * MOTOR_COUNT; i++) {
    ellipse += ".";
  }
  
  String instructionArgs[1] = {ellipse};
  addInstruction(delayBeforeReset, INSTRUCTION_SET_DISPLAY, instructionArgs);
  addInstruction(0, INSTRUCTION_RESET_ALL, instructionArgs);
}

void resetAll() {
  Serial.println("-- RESETTING DISPLAY --");  
  for (int board = 0; board < BOARD_COUNT; board++) {
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      resetMotor(board, motor);
    }
  }

  // all motors should be set as paused, and no longer listening for button presses
  writeRegisters();
}

void resetMotor(int board, int motor) {
  // output high on the correct button register
  updateButtonRegister(board, motor);
  
  // all registers besides this one should get paused
  for (int i = 0; i < BOARD_COUNT; i++) {
    for (int j = 0; j < MOTOR_COUNT; j++) {
      if (i != board && j != motor) {
        pauseMotor(board, motor);
      }
    }
  }
  
  // write registers to output the button data before reading
  writeRegisters();
  // delay(2);

  // run the motor reset
  resetMotorPosition(board, motor);
}

//void resetAllNew() {
//  for (int board = 0; board < BOARD_COUNT; board++) {
//    // get past the button
//    stepMotorsToValue(board, LOW);
//    // get to the button
//    stepMotorsToValue(board, HIGH);
//  }
//
//  // all motors should be set as paused, and no longer listening for button presses
//  writeRegisters();
//}

//void stepMotorsToValue(int board, int buttonVal) {
//  bool allMotorsDone = false;
//  bool motorsDone[MOTOR_COUNT] = {false, false, false, false};
//  byte bugRegs[4] = {B10000000,B01000000,B00100000,B00010000};
//    
//  while (!allMotorsDone) {
//    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
//      //step all motors
//      if (!motorsDone[motor]) {
//        stepMotor(board, motor);
//      }
//    }
//
//    // move the motors
//    writeRegisters();
//    
//    // rotate through buttons and read for start
//    int motorsDoneCount = 0;
//    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
//      if (!motorsDone[motor]) {
//        startButtonRegisterOutput[board] = bugRegs[motor];
//        writeRegistersNow();
//        delay(1);
//  
//        if (digitalRead(MOTOR_START_PIN) == buttonVal) {
//          Serial.print("MOTOR DONE: ");
//          Serial.println(motor);
//          motorsDone[motor] = true;
//        }
//
//        // dont send additional instructuons this loop
//        pauseMotor(board, motor);
//      } else {
//        motorsDoneCount++;
//      }
//    }
//
//    allMotorsDone = (motorsDoneCount == MOTOR_COUNT);
//  }
//}

/**
 * Send the specified motor to its starting position
 */
void resetMotorPosition(int board, int motor) {
  // always move a couple steps to get past a button (assuming we started on one)
  if (!isButtonTriggered(board)) {
    for (int i = getStepsPerCharacter(board, motor) * 5; i > 0; i--) {
      stepMotor(board, motor);
      writeRegisters();
    }
  }

  // loop listener will be unattached through an interrupt once the start position is reached
  attachLoopListener(board, motor);
  while (isLoopListenerAttached(board)) {
    stepMotor(board, motor);
    writeRegisters();
  }

  // advance more to match the starting offset
  for (int i = CHARACTER_OFFSET[board][motor]; i > 0; i--) {
    stepMotor(board, motor);
    writeRegisters();
  }

  // reset the current character
  storeCurrentCharacter(board, motor, 0);

  // pause the motor
  pauseMotor(board, motor);

  // reset the start button register to no longer listen
  resetButtonRegister(board);
}
