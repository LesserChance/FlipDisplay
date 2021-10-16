#include "FlipDisplay_Queue.h"

const int QUEUE_SIZE = 30;
const int ARGS_SIZE = 1;
int runPointer = 0;
int addPointer = 0;

int queueDelay[QUEUE_SIZE];
int queueInstruction[QUEUE_SIZE];
String queueStringArgs[QUEUE_SIZE][ARGS_SIZE];

unsigned long nextStartTime = 0;

void executeQueueInLoop() {
  stepMovingMotors();
  writeRegisters();

  // if no motors are moving, run the next instruction
  if (allMotorsArePaused()) {
    executeNextInstruction();
  }
}

void addInstruction(int msDelay, int instruction, String stringArgs[]) {
  // if we have looped too much, dont add any more instructions
  if ((addPointer + 1) % QUEUE_SIZE == runPointer) {
    return;
  }
  
  queueDelay[addPointer] = msDelay;
  queueInstruction[addPointer] = instruction;
  for (int i = 0; i < ARGS_SIZE; i++){
    queueStringArgs[addPointer][i] = stringArgs[i];
  }

  addPointer = (addPointer + 1) % QUEUE_SIZE;
}

void executeNextInstruction() {
  // if nothing is queued, queue the next instruction with the appropriate delay
  if (runPointer != addPointer && nextStartTime == 0) {
    unsigned long currentTime = millis();
    
    nextStartTime = currentTime + queueDelay[runPointer];
  }

  // check to see if the next instructions delay time has passed
  if (nextStartTime != 0) {
    unsigned long currentTime = millis();
    if (currentTime >= nextStartTime) {
      switch (queueInstruction[runPointer]) {
        case INSTRUCTION_SET_DISPLAY:
          setDisplayString(queueStringArgs[runPointer][0]);
          break;
        case INSTRUCTION_RESET_ALL:
          resetAll();
          break;
      }

      nextStartTime = 0;
      runPointer = (runPointer + 1) % QUEUE_SIZE;
    }
  }
}
