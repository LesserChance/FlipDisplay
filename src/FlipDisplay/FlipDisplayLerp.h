#ifndef FlipDisplayLerp_h
#define FlipDisplayLerp_h

#include <Arduino.h>
#include <stdint.h>

#include "Config.h"

class FlipDisplayLerp {
    public:
        static const int C_MIN_STEP_DURATION = MIN_STEP_DURATION;
        static const int C_AVG_STEP_DURATION = AVG_STEP_DURATION;
        static const int C_HOMING_STEP_DURATION = HOMING_STEP_DURATION;

        typedef enum {
            FLAT = 0,             // stay the same speed the whole time
            LINEAR_SPEED_UP = 1,  // start slow, get fast, with the most variation possible
            LINEAR_SLOW_DOWN = 2, // start fast, get slow, with the most variation possible
            RAMP_UP_MAX = 3,      // fastest in the middle, with the most variation possible
            RAMP_DOWN_MAX = 4,    // slowest in the middle, with the most variation possible
        } LerpType;

        FlipDisplayLerp(int steps = 0, LerpType type = LerpType::FLAT, unsigned long duration = 0);
        unsigned long getTimeToNextStep();
        void setDuration(unsigned long duration);
        void setDelay(unsigned long delay);
        
        int getTotalSteps();
        unsigned long getTotalDuration();
        unsigned long getLastStepDuration();
        unsigned long getAverageStepDuration();
        void addLoop();
        bool isComplete();
        void debug();

        static unsigned long getMaxDuration(FlipDisplayLerp * lerps, int lerpCount);

    private:
        LerpType _type;

        unsigned long _delay;
        unsigned long _totalDuration;
        unsigned long _lastStepDuration;
        int _totalSteps;
        int _pointer;
        int _scale;  // value between 0->100 to offset values by (percentage)

        unsigned long getStepDurationSpeedUp(int step);
        unsigned long getStepDurationSlowDown(int step);
        unsigned long getStepDurationRampUpMax(int step);
        unsigned long getStepDurationRampDownMax(int step);
        unsigned long getStepDurationFlat();

};

#endif