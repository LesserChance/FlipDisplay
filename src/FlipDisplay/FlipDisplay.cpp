// FlipDisplay.cpp

#include "FlipDisplay.h"

FlipDisplay::FlipDisplay() {
    pinMode(CHAR_ONE_BUTTON_PIN, INPUT);
    pinMode(CHAR_TWO_BUTTON_PIN, INPUT);
    pinMode(CHAR_THREE_BUTTON_PIN, INPUT);
    pinMode(CHAR_FOUR_BUTTON_PIN, INPUT);
    pinMode(CHAR_FIVE_BUTTON_PIN, INPUT);
    pinMode(CHAR_SIX_BUTTON_PIN, INPUT);

    _characters[0] = new FlipDisplayCharacter(CHAR_ONE_BUTTON_PIN, 82);
    _characters[1] = new FlipDisplayCharacter(CHAR_TWO_BUTTON_PIN, 78);
    _characters[2] = new FlipDisplayCharacter(CHAR_THREE_BUTTON_PIN, 110);
    _characters[3] = new FlipDisplayCharacter(CHAR_FIVE_BUTTON_PIN, 118);
    _characters[4] = new FlipDisplayCharacter(CHAR_SIX_BUTTON_PIN, 110);

    _type = AnimationType::ARRIVE_TOGETHER;
}

void FlipDisplay::home() {
    for (int i = 0; i < CHARACTER_COUNT; i++) {
        _characters[i]->home();
    }
}

void FlipDisplay::run() {
    _currentTime = micros();
    checkForScroll();

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        _characters[i]->run();
    }
}

void FlipDisplay::checkForScroll() {
    if (_nextScrollTime != 0 && _currentTime > _nextScrollTime) {
        _currentDisplayScrollPosition++;
        lerpToCurrentDisplay();
    }
}

byte FlipDisplay::getRegisterOutput() {
    byte stepPinRegisterOutput = 0b00000000;
    bitWrite(stepPinRegisterOutput, 1, _characters[0]->getStepPinValue());
    bitWrite(stepPinRegisterOutput, 2, _characters[1]->getStepPinValue());
    bitWrite(stepPinRegisterOutput, 3, _characters[2]->getStepPinValue());
    bitWrite(stepPinRegisterOutput, 5, _characters[3]->getStepPinValue());
    bitWrite(stepPinRegisterOutput, 6, _characters[4]->getStepPinValue());

    return stepPinRegisterOutput;
}

void FlipDisplay::setDisplay(String displayString) {
    displayString.toUpperCase();
    _currentDisplay = displayString;
    _currentDisplayScrollPosition = 0;
    
    Serial.print("SETTING DISPLAY TO: ");
    Serial.println(_currentDisplay);
    
    lerpToCurrentDisplay();
}

void FlipDisplay::lerpToCurrentDisplay() {
    String displayString = _currentDisplay.substring(_currentDisplayScrollPosition, _currentDisplayScrollPosition + CHARACTER_COUNT);

    FlipDisplayLerp lerps[CHARACTER_COUNT];

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        char displayChar = (displayString.length() > i) ? displayString[i] : ' ';
        lerps[i] = FlipDisplayLerp(_characters[i]->getStepsToChar(displayChar), FlipDisplayLerp::LerpType::FLAT);
    }
    
    unsigned long maxDuration = FlipDisplayLerp::getMaxDuration(lerps, 5);

    // mutate the lerps based on the animation type
    switch(_type) {
        case AnimationType::ARRIVE_TOGETHER:
            for (int i = 0; i < CHARACTER_COUNT; i++) {
                lerps[i].setDelay(maxDuration - lerps[i].getTotalDuration());
            }
            break;

        case AnimationType::START_TOGETHER:
        default:
            break;
    }
    
    for (int i = 0; i < CHARACTER_COUNT; i++) {
        _characters[i]->startLerp(lerps[i]);
    }

    // Determine if we need to scroll
    if (_currentDisplay.length() > _currentDisplayScrollPosition + CHARACTER_COUNT){
        _nextScrollTime = _currentTime + maxDuration + WAIT_TO_SCROLL;
    } else {
        _nextScrollTime = 0;
    }
}