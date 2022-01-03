#ifndef FlipDisplayCharacter_h
#define FlipDisplayCharacter_h

#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>

#include "FlipDisplayConfig.h"
#include "FlipDisplayLerp.h"

class FlipDisplayCharacter {
   public:
    typedef enum {
        // homing, running to get past the button
        HOMING_PAST_BUTTON = 0,
        // homing, running to get to the button
        HOMING_TO_BUTTON = 1,
        // homing, running to the offset amount
        HOMING_TO_OFFSET = 2,
        // actively moving
        RUNNING = 3,
        // paused
        PAUSED = 4,
        // paused for long enough that its disabled
        MOTOR_OFF = 5,
    } FlipDisplayCharacterState;

    FlipDisplayCharacter(int characterIndex, uint8_t startOffset = 0);

    /**
     * @brief the main loop function
     * Determine if the character needs to step or move to a different state
     * @return FlipDisplayCharacterState the new state thats been determined
     */
    FlipDisplayCharacterState run();

    /**
     * @brief start character movement
     * @param lerp the lerp controls the speed/duration
     */
    void startLerp(FlipDisplayLerp lerp);

    /**
     * @brief Re-home the character
     */
    void home();

    /**
     * @brief stop moving the character
     */
    void pause();

    /**
     * @brief step the character by a certain amount
     * @param steps the number of steps to move
     */
    void step(int steps);

    /**
     * @brief if the character hits its target, allow it to keep going around
     * This is used when we need to force a loop to happen to reset the 0
     * position. if this is true when the character is reached, it gets set to
     * false so itll stop next time around
     */
    void allowLoop();

    /**
     * @brief set a new button state if its been debounced
     * the FlipDisplay handles reading the button state, and calls this whenever
     * it changes. This function debounces it appropriately
     * @param value LOW or HIGH
     */
    void debounceButtonState(uint8_t value);

    /**
     * @brief Set the button state
     * @param value LOW or HIGH
     */
    void setButtonState(uint8_t value);

    /**
     * @brief Get the state
     * @return FlipDisplayCharacterState
     */
    FlipDisplayCharacterState getState();

    /**
     * @brief Get the value for the pin that controls the motor movement
     * @return uint8_t LOW or HIGH
     */
    uint8_t getStepPinValue();

    /**
     * @brief determine the number of steps to get to the target
     * Given steps per character, determine how many steps we need to move to
     * get to the target character. This will inject a loop into the number of
     * steps if the motor is currently off
     * @param targetChar
     * @return int
     */
    int getStepsToChar(char targetChar);

    /**
     * @brief how far are we currently from the button initially going HIGH
     * @return int
     */
    int getCurrentOffsetFromButton();

   private:
    /*************************************
     * PROPERTIES
     *************************************/
    // what characters can this display
    const char* _characters = " ABCDEFGHIJKLMNOPQRSTUVWXYZ23456789()'.-";

    // the current state of the character
    FlipDisplayCharacterState _state;

    // each step needs to output a 1 then a 0
    uint8_t _phase;

    // if true, will allow the character to keep going when it hits te
    // destination
    bool _allowLoop = false;

    // which character within the display are we controlling
    int _characterIndex;

    // current step (out of STEPS_PER_REVOLUTION)
    int _currentPosition;

    // target step (out of STEPS_PER_REVOLUTION)
    int _targetPosition;

    // the last time (microseconds) update() was called
    unsigned long _currentTime;

    // the time (microseconds) we first had a button state change
    unsigned long _debounceStart;

    // the time the character started being paused
    unsigned long _pausedTime;

    // value read from the button pin
    uint8_t _buttonPinValue;

    // value read from the button pin on the last run loop
    uint8_t _prevButtonPinValue;

    // value to output to the step pin
    uint8_t _stepPinValue;

    // how far to offset from the button state change to reach the start point
    // (steps)
    uint8_t _startOffset;

    // the next time (microseconds) the character can step
    unsigned long _nextStepTime;

    // the currently executing lerp
    FlipDisplayLerp _lerp;

    /*************************************
     * METHODS
     *************************************/

    /**
     * @brief keep the motor running until a button state is reached
     * @param targetButtonState LOW or HIGH
     */
    void runToButtonState(uint8_t targetButtonState);

    /**
     * @brief while the motor is running, determine if a loop has occurred
     */
    void detectLoop();

    /**
     * @brief step the motor if we're supposed to (based on the current lerp)
     */
    void stepOnTime();

    /**
     * @brief based on the current state, time and the lerp, calculate the next
     * time we should step
     */
    void calculateNextStepTime();

    /**
     * @brief based on the number of steps, set a target position to go to  (out
     * of STEPS_PER_REVOLUTION)
     * @param steps the number of steps to advance
     */
    void setTargetPosition(int steps);

    /**
     * @brief set the current position
     * This is used when looping to reset the position to 0
     * @param currentPosition
     */
    void setCurrentPosition(int currentPosition);

    /**
     * @brief get the index (within _characters) of the target haracter
     * @param targetChar the character
     * @return int the index
     */
    int getCharPosition(char targetChar);
};

#endif