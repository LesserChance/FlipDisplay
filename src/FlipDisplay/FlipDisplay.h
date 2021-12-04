#ifndef FlipDisplayAnimation_h
#define FlipDisplayAnimation_h

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#include "FlipDisplayCharacter.h"
#include "FlipDisplayLerp.h"

#define CHARACTER_COUNT 5  // how many charcters will the animations control

#define CHAR_ONE_BUTTON_PIN    35
#define CHAR_TWO_BUTTON_PIN    32
#define CHAR_THREE_BUTTON_PIN  33
#define CHAR_FOUR_BUTTON_PIN   25
#define CHAR_FIVE_BUTTON_PIN   26
#define CHAR_SIX_BUTTON_PIN    27

#define WAIT_TO_SCROLL 1000000 // 1 second

class FlipDisplay {
    public:
        typedef enum {
            START_TOGETHER = 0,             // all characters start at the same time
            ARRIVE_TOGETHER = 1,            // all characters arrive at the destination at the same time
        } AnimationType;

        FlipDisplay();
        
        void home();
        void run();
        byte getRegisterOutput();
        void setDisplay(String display);

    private:
        AnimationType _type;
        String _currentDisplay;

        FlipDisplayCharacter * _characters[CHARACTER_COUNT];
        unsigned long _currentTime;
        
        int _currentDisplayScrollPosition;
        unsigned long _nextScrollTime;

        void checkForScroll();
        void lerpToCurrentDisplay();
};

#endif