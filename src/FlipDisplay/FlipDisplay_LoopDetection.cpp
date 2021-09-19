#include "FlipDisplay_LoopDetection.h"

// how many loops should be allowed before resetting the zero position - higher makes it less likely for collisions
const int LOOP_RESET_THRESHOLD = 2; 

// count down from LOOP_RESET_THRESHOLD, when at 0, reset the 0 position
int loopResetCount[BOARD_COUNT][MOTOR_COUNT] = {{LOOP_RESET_THRESHOLD, LOOP_RESET_THRESHOLD, LOOP_RESET_THRESHOLD, LOOP_RESET_THRESHOLD}};

// board, motor, charIndex
int detectingLoop[3] = {-1, -1, -1};

volatile bool listenForStartTrigger = false;
volatile unsigned long startTriggerDebounce = 0;

void setupLoopDetection() {
  attachInterrupt(digitalPinToInterrupt(getMotorStartPin()), trigger_motorAtStart, FALLING);
}

void attachLoopListener() {
  listenForStartTrigger = true;
}

void unattachLoopListener() {
  listenForStartTrigger = false;
}

bool isLoopListenerAttached() {
  return listenForStartTrigger;
}

void trigger_motorAtStart() {
  if (isLoopListenerAttached() && !isButtonTriggered()) {
    // a motor we were listening to has hit its start position
    unsigned long currentTime = millis();

    // if we're debounced:
    // A) signal to the main process (during resetting)
    // B) signal that a motor that is being monitored has hit its zero position
    if (startTriggerDebounce <= currentTime - DEBOUNCE_WAIT_TIME) {
      unattachLoopListener();
      loopDetected();
    }
  
    startTriggerDebounce = currentTime;
  }
}

void reduceLoopCount(int board, int motor, int targetIndex) {
  loopResetCount[board][motor] = loopResetCount[board][motor] - 1;

  if (loopResetCount[board][motor] <= 0) {
    bool detecting = setDetectingLoop(board, motor, targetIndex);
    if (detecting) {
      attachLoopListener();
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
  if (detectingLoop[0] == -1) {
    // ignore, we arent detecting a loop
    return;
  }
  
  int board = detectingLoop[0];
  int motor = detectingLoop[1];
  int targetIndex = detectingLoop[2];
  
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
