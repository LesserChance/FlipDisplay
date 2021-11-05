#include "FlipDisplay_Queue.h"

const int QUEUE_SIZE = 30;
const int ARGS_SIZE = 1;
int runPointer = 0;
int addPointer = 0;

int queueDelay[QUEUE_SIZE];
int queueInstruction[QUEUE_SIZE];
String queueStringArgs[QUEUE_SIZE][ARGS_SIZE];

unsigned long nextStartTime = 0;
unsigned long nextResetTime = 0;

boolean isReset = true;
String lastTime = "";

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
  unsigned long currentTime = millis();
    
  // if nothing is queued, queue the next instruction with the appropriate delay
  if (runPointer != addPointer && nextStartTime == 0) {
    nextStartTime = currentTime + queueDelay[runPointer];
  }

  // check to see if the next instructions delay time has passed
  if (nextStartTime != 0) {
    if (currentTime >= nextStartTime) {
      switch (queueInstruction[runPointer]) {
        case INSTRUCTION_SET_DISPLAY:
          setDisplayString(queueStringArgs[runPointer][0]);

          // if this was queued during a reset wait, start that timeout from 0
          isReset = false;
          nextResetTime = 0;
          break;
        case INSTRUCTION_RESET_ALL:
          resetAll();
          isReset = true;
          break;
      }

      nextStartTime = 0;
      runPointer = (runPointer + 1) % QUEUE_SIZE;
    }
  } else if (!isReset) {
    // there are no instructions queued, we can queue up a reset after some delay, but let other instructions interrupt that
    // todo: this isnt a great way of allowing interrupts to happen, we should be able to set this 30s in the future
    // and still have other instructions occur (then just delay that by 30s) - maybe, if we add an instruction with a 0 delay it
    // just jumps the queue, then, we could just do queueReset(3000); and other interrupts would still trigger - no need 
    // for time math here!
    if (nextResetTime == 0) {
      Serial.println("NO INSTRUCTIONS, WAITING TO RESET");
      nextResetTime = currentTime + WORD_PAUSE_TIME;
    }
    if (currentTime >= nextResetTime) {
      Serial.println("DONE WAITING");
      queueReset(0);
      nextResetTime = 0;
    }
    
  } else {
    // just display the time, we have nothing else to do
    String now = getTime();
    if (lastTime != now) {
        lastTime = now;
        setDisplayString(now);
    }
    
  }
}
