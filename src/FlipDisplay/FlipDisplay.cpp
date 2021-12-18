// FlipDisplay.cpp

#include "FlipDisplay.h"

FlipDisplay::FlipDisplay() {
    for (int i = 0; i < CHARACTER_COUNT; i++) {
        _characters[i] = new FlipDisplayCharacter(i, CHARACTER_OFFSET[i]);
    }

    _type = AnimationType::ARRIVE_TOGETHER;
}

void FlipDisplay::home() {
    if (_isHoming) {
#if DEBUG
    Serial.println("HOMING ALREADY IN PROGRESS, NOT STARTNG");
#endif
        return;
    }
    
#if DEBUG
    Serial.println("HOMING START");
#endif
    _isHoming = true;
    readInButtonRegister();
    
    for (int i = 0; i < CHARACTER_COUNT; i++) {
        uint8_t buttonState = !bitRead(_buttonRegisterInput, i);
        _characters[i]->setButtonState(buttonState);
        _characters[i]->home();
    }
}

void FlipDisplay::run() {
    _currentTime = micros();
    bool isHoming = false;

    checkForScroll();

    int pauseCount = 0;
    for (int i = 0; i < CHARACTER_COUNT; i++) {
        FlipDisplayCharacter::FlipDisplayCharacterState characterState = _characters[i]->run();

        switch (characterState) {
            case FlipDisplayCharacter::FlipDisplayCharacterState::HOMING_PAST_BUTTON:
            case FlipDisplayCharacter::FlipDisplayCharacterState::HOMING_TO_BUTTON:
            case FlipDisplayCharacter::FlipDisplayCharacterState::HOMING_TO_OFFSET:
                // true as long as at least one character is homing
                isHoming = true;
                break;

            default:
                break;
        }
    }

    if (_isHoming && !isHoming) {
        _isHoming = isHoming;
#if DEBUG
        Serial.println("HOMING COMPLETE");
#endif
    }

    updateRegisters();
}

void FlipDisplay::updateRegisters() {
    byte stepPinRegisterOutput = getRegisterOutput();
    if (_lastStepPinRegisterOutput != stepPinRegisterOutput) {
        _lastStepPinRegisterOutput = stepPinRegisterOutput;

        for (int i = 0; i < CHARACTER_COUNT; i++) {
            digitalWrite(STEP_PIN[i], bitRead(stepPinRegisterOutput, i));
        }
            
        if (_isHoming) {
            readInButtonRegister();
            
            for (int i = 0; i < CHARACTER_COUNT; i++) {
                uint8_t buttonState = !bitRead(_buttonRegisterInput, i);
                _characters[i]->debounceButtonState(buttonState);
            }
        }
    }
}

void FlipDisplay::readInButtonRegister() {
    _buttonRegisterInput = 0b0000000;

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        bitWrite(_buttonRegisterInput, i, digitalRead(LOOP_PIN[i]));
    }
}

void FlipDisplay::checkForScroll() {
    if (SCROLLING_ENABLED && _nextScrollTime != 0 && _currentTime > _nextScrollTime) {
        _currentDisplayScrollPosition += CHARS_TO_SCROLL;

#if DEBUG
    Serial.print("SCROLLING TO POSITION: ");
    Serial.println(_currentDisplayScrollPosition);
#endif

        lerpToCurrentDisplay();
    }
}

byte FlipDisplay::getRegisterOutput() {
    byte stepPinRegisterOutput = 0b00000000;
    for (int i = 0; i < CHARACTER_COUNT; i++) {
        bitWrite(stepPinRegisterOutput, i, _characters[i]->getStepPinValue());
    }

    return stepPinRegisterOutput;
}

void FlipDisplay::setDisplay(String displayString) {
    displayString.toUpperCase();
    _currentDisplay = displayString;
    _currentDisplayScrollPosition = 0;
    
#if DEBUG
    Serial.print("SETTING DISPLAY TO: ");
    Serial.println(_currentDisplay);
#endif

    lerpToCurrentDisplay();
}

void FlipDisplay::lerpToCurrentDisplay() {
    String displayString = _currentDisplay.substring(_currentDisplayScrollPosition, _currentDisplayScrollPosition + CHARACTER_COUNT);

    FlipDisplayLerp lerps[CHARACTER_COUNT];

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        char displayChar = (displayString.length() > i) ? displayString[i] : ' ';
        lerps[i] = FlipDisplayLerp(_characters[i]->getStepsToChar(displayChar), FlipDisplayLerp::LerpType::FLAT);
    }
    
    unsigned long maxDuration = FlipDisplayLerp::getMaxDuration(lerps, CHARACTER_COUNT);

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

    if (SCROLLING_ENABLED) {
        // Determine if we need to scroll
        if (_currentDisplay.length() > _currentDisplayScrollPosition + CHARACTER_COUNT){
            _nextScrollTime = _currentTime + maxDuration + WAIT_TO_SCROLL;
        } else {
            _nextScrollTime = 0;
        }
    }
}