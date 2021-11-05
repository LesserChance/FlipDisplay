#include "FlipDisplay_LoopDetection.h"

// count down from LOOP_RESET_THRESHOLD, when at 0, reset the 0 position
int loopResetCount[BOARD_COUNT][MOTOR_COUNT];

// board, motor, charIndex
int detectingLoop[3] = {-1, -1, -1};

const int FULL_RESET_ON_LOOP = 10;
int totalLoops = 0;

volatile bool listenForStartTrigger[BOARD_COUNT];
volatile unsigned long startTriggerDebounce = 0;
volatile int triggerMotor[BOARD_COUNT];

void setupLoopDetection() {
  attachInterrupt(digitalPinToInterrupt(getMotorStartPin(0)), trigger_motorAtStart0, FALLING);
//  attachInterrupt(digitalPinToInterrupt(getMotorStartPin(1)), trigger_motorAtStart1, FALLING);

  for (int board = 0; board < BOARD_COUNT; board++) {
    listenForStartTrigger[board] = false;
    listenForStartTrigger[board] = -1;
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      loopResetCount[BOARD_COUNT][MOTOR_COUNT] = LOOP_RESET_THRESHOLD;
    }
  }
}

void attachLoopListener(int board, int motor) {
  Serial.println("waiting to loop");
  Serial.print("board:");
  Serial.println(board);
  Serial.print("motor:");
  Serial.println(motor);
  listenForStartTrigger[board] = true;
  triggerMotor[board] = motor;
}

void unattachLoopListener(int board) {
  listenForStartTrigger[board] = false;
  triggerMotor[board] = -1;
}

bool isLoopListenerAttached(int board) {
  return listenForStartTrigger[board];
}

void trigger_motorAtStart0() {
  trigger_motorAtStart(0);
}

void trigger_motorAtStart1() {
  trigger_motorAtStart(1);
}

void trigger_motorAtStart(int board) {
  if (isLoopListenerAttached(board) && isButtonTriggered(board)) {
    Serial.println("trigger_motorAtStart");
    Serial.print("board: ");
    Serial.println(board);
    Serial.print("motor: ");
    Serial.println(triggerMotor[board]);
    
    // a motor we were listening to has hit its start position
    unsigned long currentTime = millis();

    // if we're debounced:
    // A) signal to the main process (during resetting)
    // B) signal that a motor that is being monitored has hit its zero position
    if (startTriggerDebounce <= currentTime - DEBOUNCE_WAIT_TIME) {
      unattachLoopListener(board);
      loopDetected();
    }
  
    startTriggerDebounce = currentTime;
  }
}

void reduceLoopCount(int board, int motor, int targetIndex) {
//  Serial.print("reduceLoopCount: ");
//  Serial.print(board);
//  Serial.print(", ");
//  Serial.println(motor);
  loopResetCount[board][motor] = loopResetCount[board][motor] - 1;
  
  totalLoops++;

  if (loopResetCount[board][motor] <= 0) {
    bool detecting = setDetectingLoop(board, motor, targetIndex);
    if (detecting) {
      attachLoopListener(board, motor);
      updateButtonRegister(board, motor);
    } else {
      // another loop was already being detected, detect this loop the next time around
      loopResetCount[board][motor] = loopResetCount[board][motor] + 1;
    }
  }
}

bool setDetectingLoop(int board, int motor, int targetIndex) {
  if (detectingLoop[0] != -1 && board != -1) {
    // we dont want to override a current detection, so exit
    return false;
  }
  
  detectingLoop[0] = board;
  detectingLoop[1] = motor;
  detectingLoop[2] = targetIndex;

  return true;
}

void loopDetected() {
  Serial.println("loopDetected");
  if (detectingLoop[0] == -1) {
    // ignore, we arent detecting a loop
    return;
  }
  
  int board = detectingLoop[0];
  int motor = detectingLoop[1];
  int targetIndex = detectingLoop[2];
  
  Serial.print("loopDetected: ");
  Serial.print(board);
  Serial.print(", ");
  Serial.println(motor);
  
  // stop detecting a loop
  setDetectingLoop(-1, -1, -1);

  // reset the loop counter
  loopResetCount[board][motor] = LOOP_RESET_THRESHOLD;

  // reset the current character
  storeCurrentCharacter(board, motor, 0);
  
  // reset how many steps to go now
  setCharacter(board, motor, CHARACTERS.charAt(targetIndex));

  //add the start offset
  addOffsetSteps(board, motor);

  // reset the start button register to no longer listen
  resetButtonRegister(board);
  
}