#include "FlipDisplay_CharacterControl.h"
#include "FlipDisplay_Config.h"
#include "FlipDisplay_Registers.h"
#include "FlipDisplay_Reset.h"
#include "FlipDisplay_LoopDetection.h"
#include "FlipDisplay_Wifi.h"

// debugging new strings
int nextTrigger = 0;
int nextChar = 0;

void setup() {
  Serial.begin(115200);
  
  connectWifi();
  setupRegisters();
  setupLoopDetection();

  // set the motors to a known position
  resetAll();
  // resetAllNew(); // this worked in 25s vs 32s for resetAll, but its fickle -- need to figure out how to make sure we start past the button

  delay(5000);

  nextTrigger = millis();
  nextChar = 35;
}

void loop() {
  if (millis() > nextTrigger) {
    requestWord();

    nextTrigger = millis() + 10000; // triger another request in ten seconds

    // String setTo = CHARACTERS.substring(nextChar, nextChar + 1) + "";
    // setTo = setTo + setTo;//+ setTo + setTo;
    // setDisplayString(setTo);
    // nextChar--;
    // if (nextChar < 0) {
    //   nextChar = 35;
    // }
  }
  
  stepMovingMotors();
  writeRegisters();
}
