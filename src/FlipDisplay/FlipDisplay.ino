#include "FlipDisplay_CharacterControl.h"
#include "FlipDisplay_Config.h"
#include "FlipDisplay_Queue.h"
#include "FlipDisplay_Registers.h"
#include "FlipDisplay_Reset.h"
#include "FlipDisplay_Time.h"
#include "FlipDisplay_LoopDetection.h"
#include "FlipDisplay_Wifi.h"

// debugging new strings
int nextTrigger = 0;
int nextChar = 0;

volatile unsigned long startButtonDebounce = 0;

// put your favorite four-letter words here
const int WORD_COUNT = 1;
String words[WORD_COUNT] = {"poop"};

void setup() {
  Serial.begin(115200);

  // temporary using this for button input
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);

  attachInterrupt(digitalPinToInterrupt(13), trigger_Word, FALLING);
  
  connectWifi();
  setTimeByNTP();
  setupCharacterControl();
  setupRegisters();
  setupLoopDetection();

  // set the motors to a known position
  resetAll();
  // resetAllNew(); // this worked in 25s vs 32s for resetAll, but its fickle -- need to figure out how to make sure we start past the button
}

void trigger_Word() {
  // button has been pressed
  unsigned long currentTime = millis();
  if (currentTime > nextTrigger) {
    // choose a random word and queue its display
    String instructionArgs[1] = {words[random(0, WORD_COUNT)]};
    addInstruction(0, INSTRUCTION_SET_DISPLAY, instructionArgs);

    // debounce the button
    nextTrigger = currentTime + 500;
  }
}

void loop() {
  executeQueueInLoop();
}
