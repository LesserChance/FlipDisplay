#ifndef FlipDisplayLerp_h
#define FlipDisplayLerp_h

#include <Arduino.h>
#include <stdint.h>

#include "Config.h"

class FlipDisplayLerp {
   public:
    typedef enum {
        // stay the same speed the whole time
        FLAT = 0,
        // start slow, get fast, with the most variation possible
        LINEAR_SPEED_UP = 1,
        // start fast, get slow, with the most variation possible
        LINEAR_SLOW_DOWN = 2,
        // fastest in the middle, with the most variation possible
        RAMP_UP_MAX = 3,
        // slowest in the middle, with the most variation possible
        RAMP_DOWN_MAX = 4,
    } LerpType;

    FlipDisplayLerp(int steps = 0, LerpType type = LerpType::FLAT,
                    unsigned long duration = 0);

    /**
     * @brief calculate the duration to wait until the nest step should occur
     * @return unsigned long
     */
    unsigned long getTimeToNextStep();

    /**
     * @brief add a number of steps to this lerp to make it do one more full
     * loop
     */
    void addLoop();

    /**
     * @brief get the total number of steps for this lerp
     * @return int
     */
    int getTotalSteps();

    /**
     * @brief set the duration of the lerp
     * All the steps will be adjusted to make this duration, based on the lerp
     * type
     * @param duration
     */
    void setDuration(unsigned long duration);

    /**
     * @brief set a new delay amount
     * delays will cause the lerp to pause before it kicks off
     * @param delay
     */
    void setDelay(unsigned long delay);

    /**
     * @brief excluding delay, get the total duration of the lerp
     * @return unsigned long
     */
    unsigned long getTotalDuration();

    /**
     * @brief the the duration of the most recently moved step
     * This is used if the character needs to keep moving even when the lerp is
     * complete
     * @return unsigned long
     */
    unsigned long getLastStepDuration();

    /**
     * @brief get the average duration of a step
     * @return unsigned long
     */
    unsigned long getAverageStepDuration();

    /**
     * @brief determine if the lerp is complete
     * @return true if the lerp no longer needs to step
     * @return false if the lerp still needs to step
     */
    bool isComplete();

    /**
     * @brief output some debug information
     */
    void debug();

    /**
     * @brief given a list of lerps, return the longest duration
     * @param lerps list of lerps
     * @param lerpCount how many lerps are in the list
     * @return unsigned long the longest duration
     */
    static unsigned long getMaxDuration(FlipDisplayLerp* lerps, int lerpCount);

   private:
    /*************************************
     * PROPERTIES
     *************************************/
    // the lerp type (determines step length over time)
    LerpType _type;

    // how long to delay before starting movement
    unsigned long _delay;

    // the duration of the lerp (excluding delay)
    unsigned long _totalDuration;

    //  the the duration of the most recently moved step
    unsigned long _lastStepDuration;

    // the total number of steps to move in this lerp
    int _totalSteps;

    // the current step of the lerp
    int _pointer;

    // value between 0->100 to offset values by (percentage)
    int _scale;

    /*************************************
     * METHODS
     *************************************/
    /**
     * @brief calculate the step duration for the current step of this animation
     * @return unsigned long
     */
    unsigned long getStepDurationSpeedUp();

    /**
     * @brief calculate the step duration for the current step of this animation
     * @return unsigned long
     */
    unsigned long getStepDurationSlowDown();

    /**
     * @brief calculate the step duration for the current step of this animation
     * @return unsigned long
     */
    unsigned long getStepDurationRampDownMax();

    /**
     * @brief calculate the step duration for the current step of this animation
     * @return unsigned long
     */
    unsigned long getStepDurationRampUpMax();

    /**
     * @brief calculate the step duration for the current step of this animation
     * @return unsigned long
     */
    unsigned long getStepDurationFlat();
};

#endif