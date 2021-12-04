#ifndef FlipDisplayCharacter_h
#define FlipDisplayCharacter_h

#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>

#include "FlipDisplayLerp.h"

#define DEBOUNCE_DURATION 1000
#define STEPS_PER_REVOLUTION 800
#define POSSIBLE_CHARACTER_VALUES 40
#define STEPS_PER_CHARACTER 20
#define PAUSE_ENABLED_DURATION 5000000 // 5 seconds

class FlipDisplayCharacter {
   public:
    typedef enum {
        HOMING_PAST_BUTTON = 0,  // homing, running to get past the button
        HOMING_TO_BUTTON = 1,    // homing, running to get to the button
        HOMING_TO_OFFSET = 2,    // homing, running to the offset amount
        RUNNING = 3,             // actively moving
        PAUSED = 4,              // paused
        MOTOR_OFF = 5,           // paused for long enough that its disabled
    } FlipDisplayCharacterState;

    FlipDisplayCharacter(uint8_t buttonPin, uint8_t startOffset = 0);

    void run();

    void startLerp(FlipDisplayLerp lerp);
    void home();
    void pause();

    void step(int steps);

    uint8_t getStepPinValue();
    uint8_t getEnablePinValue();
    int getStepsToChar(char targetChar);

   private:
    const char * _characters = " ABCDEFGHIJKLMNOPQRSTUVWXYZ23456789()'.-";
    
    uint8_t _state;               // the current state of the character
    uint8_t _phase;               // each step needs to output a 1 then a 0

    int _currentPosition;         // current step (out of STEPS_PER_REVOLUTION)
    int _targetPosition;          // target step (out of STEPS_PER_REVOLUTION)

    unsigned long _currentTime;   // the last time (microseconds) update() was called
    unsigned long _debounceStart; // the time (microseconds) we first had a button state change
    unsigned long _pausedTime;    // the time the digit started being paused

    uint8_t _buttonPin;           // pin to read button input from

    uint8_t _stepPinValue;        // value to output to the step pin
    uint8_t _enablePinValue;      // value to output to the enable pin (LOW is ENABLED)
    uint8_t _startOffset;         // how far to offset from the button state change to reach the start point (steps)

    unsigned long _nextStepTime;  // the next time (microseconds) the character can step

    FlipDisplayLerp _lerp;        // the currently executing lerp

    void runToButtonState(uint8_t targetButtonState);
    void stepOnTime();
    void setTargetPosition(int steps);
    void setCurrentPosition(int currentPosition);
    int getCharPosition(char targetChar);
};

#endif