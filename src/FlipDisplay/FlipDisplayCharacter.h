#ifndef FlipDisplayCharacter_h
#define FlipDisplayCharacter_h

#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>

#include "Config.h"
#include "FlipDisplayLerp.h"


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

    FlipDisplayCharacter(int characterIndex, uint8_t startOffset = 0);

    FlipDisplayCharacterState run();

    void startLerp(FlipDisplayLerp lerp);
    void home();
    void pause();

    void step(int steps);

    uint8_t getStepPinValue();
    uint8_t getEnablePinValue();
    void debounceButtonState(uint8_t value);
    void setButtonState(uint8_t value);
    int getStepsToChar(char targetChar);

   private:
    const char * _characters = " ABCDEFGHIJKLMNOPQRSTUVWXYZ23456789()'.-";
    
    FlipDisplayCharacterState _state; // the current state of the character
    uint8_t _phase;               // each step needs to output a 1 then a 0

    int _characterIndex;
    int _currentPosition;         // current step (out of STEPS_PER_REVOLUTION)
    int _targetPosition;          // target step (out of STEPS_PER_REVOLUTION)

    unsigned long _currentTime;   // the last time (microseconds) update() was called
    unsigned long _debounceStart; // the time (microseconds) we first had a button state change
    unsigned long _pausedTime;    // the time the character started being paused

    uint8_t _buttonPinValue;      // value read from the button pin
    uint8_t _stepPinValue;        // value to output to the step pin
    uint8_t _enablePinValue;      // value to output to the enable pin (LOW is ENABLED)
    uint8_t _startOffset;         // how far to offset from the button state change to reach the start point (steps)

    unsigned long _nextStepTime;  // the next time (microseconds) the character can step

    FlipDisplayLerp _lerp;        // the currently executing lerp    

    void runToButtonState(uint8_t targetButtonState);
    void stepOnTime();
    void calculateNextStepTime();
    void setTargetPosition(int steps);
    void setCurrentPosition(int currentPosition);
    int getCharPosition(char targetChar);
};

#endif