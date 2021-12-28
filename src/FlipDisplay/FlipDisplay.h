#ifndef FlipDisplay_h
#define FlipDisplay_h

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#include "Config.h"
#include "FlipDisplayCharacter.h"
#include "FlipDisplayLerp.h"

class FlipDisplay {
    public:
        typedef enum {
            START_TOGETHER = 0,             // all characters start at the same time
            ARRIVE_TOGETHER = 1,            // all characters arrive at the destination at the same time
            SAME_DURATION = 2,              // all characters turn for the same duration (variable speed)

            CASCADE_LEFT = 3,
            CASCADE_RIGHT = 4              
        } AnimationType;

        FlipDisplay();
        
        void home();
        void run();
        byte getRegisterOutput();
        int stepCharacter(int characterIndex);
        void setDisplay(String display);
        void setAnimationType(AnimationType type);

        void enable(bool force = false);
        void disable(bool force = false);

    private:
        AnimationType _type;
        AnimationType _setAnimationType;
        AnimationType _scrollAnimationType;

        String _currentDisplay;

        FlipDisplayCharacter * _characters[CHARACTER_COUNT];
        unsigned long _currentTime;
        
        int _currentDisplayScrollPosition;
        unsigned long _nextScrollTime;

        bool _isHoming = false;
        bool _isDisabled = false;
        
        byte _lastStepPinRegisterOutput = 0b00000000;
        byte _buttonRegisterInput = 0b00000000;

        void readInButtonRegister();
        void updateRegisters();

        void checkForScroll();
        void checkForEnable();
        void lerpToCurrentDisplay();
};

#endif