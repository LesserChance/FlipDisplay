#include "FlipDisplay_CharacterControl.h"

// which character index is the motor currently showing
int currentCharacter[BOARD_COUNT][MOTOR_COUNT];

// which character index in the string does this motor control
int characterIndex[BOARD_COUNT][MOTOR_COUNT];

// keep track of the number of steps left to move
int remainingSteps[BOARD_COUNT][MOTOR_COUNT];

void setupCharacterControl() {
  for (int board = 0; board < BOARD_COUNT; board++) {
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      currentCharacter[board][motor] = 0;
      characterIndex[board][motor] = (board * MOTOR_COUNT) + motor;
      remainingSteps[board][motor] = 0;
    }
  }
}

void setDisplayString(String newString) {
  Serial.println("-- UPDATING DISPLAY --");
  Serial.println(newString);

  // make sure the string is uppercase
  newString.toUpperCase();
  for (int board = 0; board < BOARD_COUNT; board++) {
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      int index = characterIndex[board][motor];
      if (index > newString.length()) {
        setCharacter(board, motor, ' ');
      } else {
        setCharacter(board, motor, newString[index]);
      }
    }
  }
}

void setCharacter(int board, int motor, char character) {
  int newCharPos = CHARACTERS.indexOf(character);
  if (newCharPos == -1) {
    character = ' ';
    newCharPos = 0;
  }
  
  remainingSteps[board][motor] = getCharDistance(board, motor, character);
  storeCurrentCharacter(board, motor, newCharPos);
}

void storeCurrentCharacter(int board, int motor, int character) {
  currentCharacter[board][motor] = character;
}

int getStepsPerCharacter(int board, int motor) {
  return STEPS_PER_CHAR;
}

void addOffsetSteps(int board, int motor) {
  remainingSteps[board][motor] = remainingSteps[board][motor] + CHARACTER_OFFSET[board][motor];
}

int getCharDistance(int board, int motor, char target) {
  if (target == '1') {
    target = 'I';
  }
  if (target == '0') {
    target = 'O';
  }

  int currentIndex = currentCharacter[board][motor];
  int targetIndex = CHARACTERS.indexOf(target);

  //if we're passing the end, add the catch up steps
  int distance = 0;
  if (targetIndex < currentIndex) {
    distance = CATCH_UP_STEPS;

    // this is a loop, count it
    reduceLoopCount(board, motor, targetIndex);
  }

  distance += ((CHAR_COUNT + (targetIndex - currentIndex)) % CHAR_COUNT) * STEPS_PER_CHAR;
  
  return distance;
}

void stepMovingMotors() {
  // only move the ones with the max number
  for (int board = 0; board < BOARD_COUNT; board++) {
    int maxMove = 0;
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      if (remainingSteps[board][motor] > maxMove) {
        maxMove = remainingSteps[board][motor];
      }
    }
    
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      if (remainingSteps[board][motor] == 0) {
        pauseMotor(board, motor);
      } else if (remainingSteps[board][motor] >= maxMove) {
        stepMotor(board, motor);
        remainingSteps[board][motor]--;
      }
    }
  }
}

bool allMotorsArePaused() {
  for (int board = 0; board < BOARD_COUNT; board++) {
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      if (remainingSteps[board][motor] > 0) {
        return false;
      }
    }
  }

  return true;
}
