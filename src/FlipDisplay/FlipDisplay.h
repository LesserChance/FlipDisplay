#ifndef FlipDisplay_h
#define FlipDisplay_h

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#include "FlipDisplayConfig.h"
#include "FlipDisplayCharacter.h"
#include "FlipDisplayLerp.h"

class FlipDisplay {
   public:
    typedef enum {
        // all characters start at the same time
        START_TOGETHER = 0,
        // all characters arrive at the destination at the same time
        ARRIVE_TOGETHER = 1,
        // all characters turn for the same duration (variable speed)
        SAME_DURATION = 2,
        // characters cascade form left to right
        CASCADE_LEFT = 3,
        // characters cascade form right to left
        CASCADE_RIGHT = 4
    } AnimationType;

    FlipDisplay();

    /**
     * @brief initialize the display
     * @param goHome if ture, home the display immediately
     */
    void setupDisplay(bool goHome = true);

    /**
     * @brief Re-home the entire display
     * Using the buttons on each character, go past the button if on it, then to
     * the button, then to the starting offset this gets each character to a
     * zero position
     */
    void home();

    /**
     * @brief the main loop function, runs all characters
     * Run each character's loop function and given the current state of the
     * display, determine if we need to scroll, enable, or disable
     */
    void run();

    /**
     * @brief set the display
     * Switch the animation type to the "set" type, and determine all the lerps
     * necessary to get to the new value
     *
     * @param displayString the new string to set the display to
     */
    void setDisplay(String display);

    /**
     * @brief step an inidivial character once
     * @param characterIndex which character to step
     * @return int the new offset of the character
     */
    int stepCharacter(int characterIndex);

    /**
     * @brief the next time the display changes it'll use this animation type
     * @param type the type of animation
     */
    void setAnimationType(AnimationType type);

    /**
     * @brief enable the motors
     * @param force if true, the GPIO is updated, but the state is not, this can
     * be used for debugging
     */
    void enable(bool force = false);

    /**
     * @brief disable the motors
     * @param force if true, the GPIO is updated, but the state is not, this can
     * be used for debugging
     */
    void disable(bool force = false);

    /**
     * @brief restart the ESP after some delay
     * @param delay seconds to delay before restarting
     */
    void triggerRestart(int delay);

    /**
     * @brief 
     * @return true if the display has entirely finished its current string
     * @return false if any characters are currently changing or waiting to scroll
     */
    bool canDisplayBeUpdated();

   private:
    /*************************************
     * PROPERTIES
     *************************************/
    // the current animation type
    AnimationType _type;

    // the animation type used when setting a new string
    AnimationType _setAnimationType;

    // the animation type used when scrolling
    AnimationType _scrollAnimationType;

    // the full string that is currently displayed (or in progress of being
    // displayed)
    String _currentDisplay;

    // the character objects the display is controlling
    FlipDisplayCharacter* _characters[CHARACTER_COUNT];

    // the timestamp of the most recent run call
    unsigned long _currentTime;

    // the index of the first character in _currentDisplay to display
    int _currentDisplayScrollPosition;

    // the next time we should check to see if the display should scroll
    unsigned long _nextScrollTime = 0;

    // true when the display is homing at least one character
    bool _isHoming = false;

    // true when the display motors are all disabled
    bool _isDisabled = false;

    // the output for the step pins that control motor movement (at last run)
    byte _lastStepPinRegisterOutput = 0b00000000;

    // the input for the button pins that detect motor loops
    byte _buttonRegisterInput = 0b00000000;

    // the timestamp to treigger an ESP restart
    unsigned long _triggerRestart = 0;

    /*************************************
     * METHODS
     *************************************/
    /**
     * @brief read in the GPIO values from the loop detection buttons
     */
    void readInButtonRegister();

    /**
     * @brief output the GPIO values
     */
    void updateRegisters();

    /**
     * @brief determine the new GPIO values for the output register
     * @return byte
     */
    byte getRegisterOutput();

    /**
     * @brief determine if the display needs to scroll
     */
    void checkForScroll();

    /**
     * @brief determine if the display is currently homing
     */
    void checkForHoming();

    /**
     * @brief determine if the display needs to be enabled
     */
    void checkForEnable();

    /**
     * @brief determine if the display needs to be disabled
     */
    void checkForDisable();

    /**
     * @brief determine if all the characters are in the MOTOR_OFF state
     * @return true when all motors are off
     * @return false when any motors is on
     */
    bool areAllMotorsDisabled();

    /**
     * @brief start moving the characters
     * Based on the animation type, set up a lerp for each character and kick
     * them off
     */
    void lerpToCurrentDisplay();
};

#endif