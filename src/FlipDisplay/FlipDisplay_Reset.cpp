#include "FlipDisplay_Reset.h"

void queueReset(int delayBeforeReset) {
  String instructionArgs[1] = {""};
  addInstruction(0, INSTRUCTION_RESET_ALL, instructionArgs);
}

void resetAll() {
  Serial.println("-- RESETTING DISPLAY --");  

  // first step, move past the button one by one
  reset_getPastButtons();

  // second step, move to the button all at once
  reset_moveToButtons();

  // third step: advance more to match the starting offset
  reset_moveToStartPosition();
}

void reset_getPastButtons() {
  for (int board = 0; board < BOARD_COUNT; board++) {
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      updateButtonRegister(board, motor);
      writeRegisters();
      
      while (!isButtonTriggered(board)) {
        stepMotor(board, motor);
        writeRegisters();
      }

      pauseMotor(board, motor);
      resetButtonRegister(board);
    }
  }
}

void reset_moveToButtons() {
  for (int board = 0; board < BOARD_COUNT; board++) {
    bool allMotorsDone = false;
    bool motorsDone[MOTOR_COUNT];
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      motorsDone[motor] = false;
    }

    while (!allMotorsDone) {
      // pause all the motors while looping through buttons
      for (int motor = 0; motor < MOTOR_COUNT; motor++) {
        pauseMotor(board, motor);
      }

      // check if the button is pushed on each motor
      int motorsDoneCount = 0;
      for (int motor = 0; motor < MOTOR_COUNT; motor++) {
        if (motorsDone[motor]) {
          motorsDoneCount++;
        } else {
          updateButtonRegister(board, motor);
          writeRegistersWithDelay(1);

          if (!isButtonTriggered(board)) {
            motorsDone[motor] = true;
          }
        }
      }

      allMotorsDone = (motorsDoneCount == MOTOR_COUNT);
      
      // step all the moving motors on this board
      for (int motor = 0; motor < MOTOR_COUNT; motor++) {
        if (!motorsDone[motor]) {
          stepMotor(board, motor);
        }
      }
      
      writeRegisters();
    }
  }
}

void reset_moveToStartPosition() {
  for (int board = 0; board < BOARD_COUNT; board++) {
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      for (int i = CHARACTER_OFFSET[board][motor]; i > 0; i--) {
        stepMotor(board, motor);
        writeRegisters();
      }

      // reset the current character
      storeCurrentCharacter(board, motor, 0);

      // pause the motor
      pauseMotor(board, motor);
    }

    // reset the start button register to no longer listen
    resetButtonRegister(board);
  }
}