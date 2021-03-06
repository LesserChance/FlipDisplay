// FlipDisplay.cpp

#include "FlipDisplay.h"

/*************************************
 * CONSTRUCTOR
 *************************************/
FlipDisplay::FlipDisplay() {}

/*************************************
 * PUBLIC
 *************************************/

void FlipDisplay::setupDisplay(bool goHome) {
    disable(true);

    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        _characters[i] = new FlipDisplayCharacter(i, CHARACTER_OFFSET[i]);
    }

    _setAnimationType = AnimationType::ARRIVE_TOGETHER;
    _scrollAnimationType = AnimationType::CASCADE_RIGHT;

    _type = _setAnimationType;

    if (goHome) {
        run();
        home();
    }
}

void FlipDisplay::home() {
    if (_isHoming) {
#if DEBUG_LOOP
        Serial.println("HOMING ALREADY IN PROGRESS, NOT STARTNG");
#endif
        return;
    }
    
    if (!canDisplayBeUpdated()) {
#if DEBUG_LOOP
        Serial.println("FAILED TO UPDATE DISPLAY, CURRENTLY RUNNING OR PAUSED");
#endif
        return;
    }

#if DEBUG_LOOP
    Serial.println("HOMING START");
#endif
    _isHoming = true;
    readInButtonRegister();

    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        uint8_t buttonState = !bitRead(_buttonRegisterInput, i);
        _characters[i]->setButtonState(buttonState);
        _characters[i]->home();
    }
}

void FlipDisplay::run() {
    if (!_powerOn) {
        return;
    }

    unsigned long currentTime = micros();
    if (currentTime < _currentTime) {
        Serial.println("CLOCK HAS LOOPED");
        Serial.print("_currentTime: ");
        Serial.println(_currentTime);

        Serial.print("now: ");
        Serial.println(currentTime);

         // reset all counters to handle this
        _acceptNextCommand = 0;
        _nextScrollTime = 0;
    }
    
    _currentTime = currentTime;

    if (_triggerRestart) {
        if (_currentTime > _triggerRestart) {
            Serial.println("RESTARTING");
            ESP.restart();
        }
    }

    // check for any state changes we want to do before running
    checkForScroll();
    checkForEnable();
    checkForMotorsMoving();

    int pauseCount = 0;
    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        FlipDisplayCharacter::FlipDisplayCharacterState characterState =
            _characters[i]->run();
    }

    // check for any state changes we want to do after running
    checkForHoming();
    checkForDisable();

    // write all the changes to the GPIO
    updateRegisters();
}

void FlipDisplay::setDisplay(String displayString, unsigned long minDuration, bool force) {
    Serial.print("trying to set display: ");
    Serial.println(displayString);

    if (!force && !canDisplayBeUpdated()) {
#if DEBUG
        Serial.println("FAILED TO UPDATE DISPLAY, CURRENTLY RUNNING OR PAUSED");
        Serial.print("_currentTime <= _acceptNextCommand: ");
        Serial.println((_currentTime <= _acceptNextCommand));
        if (_currentTime <= _acceptNextCommand) {
            Serial.print("_currentTime: ");
            Serial.println(_currentTime);
            Serial.print("_acceptNextCommand: ");
            Serial.println(_acceptNextCommand);
        }
        Serial.print("!_areMotorsMoving: ");
        Serial.println(!_areMotorsMoving);
        Serial.print("_nextScrollTime == 0: ");
        Serial.println(_nextScrollTime == 0);
#endif
        return;
    }

    displayString.toUpperCase();
    if (_currentDisplay == displayString) {
        return;
    }

    _currentDisplay = displayString;
    _currentDisplayScrollPosition = 0;
    _minDuration = minDuration;

#if DEBUG
    Serial.print("SETTING DISPLAY TO: ");
    Serial.println(_currentDisplay);
#endif

    setAnimationType(_setAnimationType);
    calculateWords();
    lerpToCurrentDisplay();
}

int FlipDisplay::stepCharacter(int characterIndex) {
    _characters[characterIndex]->step(1);
    return _characters[characterIndex]->getCurrentOffsetFromButton() + 1;
}

void FlipDisplay::setAnimationType(AnimationType type) { _type = type; }

void FlipDisplay::enable(bool force) {
    if (force) {
        digitalWrite(enablePin, LOW);
        return;
    }

    if (_isDisabled) {
        digitalWrite(enablePin, LOW);
        _isDisabled = false;
#if DEBUG
        Serial.println("ENABLE ALL MOTORS");
#endif
    }
}

void FlipDisplay::disable(bool force) {
    if (force) {
        digitalWrite(enablePin, HIGH);
        return;
    }

    if (!_isDisabled) {
        digitalWrite(enablePin, HIGH);
        _isDisabled = true;
#if DEBUG
        Serial.println("DISABLE ALL MOTORS");
#endif
    }
}

void FlipDisplay::setPower(bool powerOn) {
    _powerOn = powerOn;
}

String FlipDisplay::getTogglePowerValue() {
    if (_powerOn) {
        return "Off";
    }

    return "On";
}

void FlipDisplay::triggerRestart(int delay) {
    Serial.println("TRIGGERING A RESTART");
    _triggerRestart = _currentTime + (delay * MICROSECONDS);
}

bool FlipDisplay::canDisplayBeUpdated() {
    if (_currentTime <= _acceptNextCommand) {
        return false;
    }

    return !_areMotorsMoving && (_nextScrollTime == 0);
}

/*************************************
 * PRIVATE
 *************************************/

void FlipDisplay::readInButtonRegister() {
    _buttonRegisterInput = 0b0000000;

    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        bitWrite(_buttonRegisterInput, i, digitalRead(LOOP_PIN[i]));
    }
}

void FlipDisplay::updateRegisters() {
    byte stepPinRegisterOutput = getRegisterOutput();
    if (_lastStepPinRegisterOutput != stepPinRegisterOutput) {
        _lastStepPinRegisterOutput = stepPinRegisterOutput;

        for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
            digitalWrite(STEP_PIN[i], bitRead(stepPinRegisterOutput, i));
        }

        readInButtonRegister();

        for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
            uint8_t buttonState = !bitRead(_buttonRegisterInput, i);
            _characters[i]->debounceButtonState(buttonState);
        }
    }
}

byte FlipDisplay::getRegisterOutput() {
    byte stepPinRegisterOutput = 0b00000000;
    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        bitWrite(stepPinRegisterOutput, i, _characters[i]->getStepPinValue());
    }

    return stepPinRegisterOutput;
}

void FlipDisplay::checkForScroll() {
    if (SCROLLING_ENABLED && _nextScrollTime != 0 && _currentTime > _nextScrollTime) {
        // if any motors are still moving, do not scroll
        if (_areMotorsMoving) {
            _nextScrollTime += WAIT_TO_SCROLL;
            return;
        }

        // determine if we should go to the next word
        if (_currentDisplayWord + 1 < _currentDisplayWordCount) {
            _currentDisplayWord++;
            lerpToCurrentDisplay();
#if DEBUG
            Serial.print("MOVING TO WORD: ");
            Serial.print(_currentDisplayWord);
            Serial.print(" (");
            Serial.print(getWord(_currentDisplayWord));
            Serial.println(")");
#endif
        }
    }


//     if (SCROLLING_ENABLED && _nextScrollTime != 0 &&
//         _currentTime > _nextScrollTime) {
//         // if any motors are still moving, do not scroll
//         if (_areMotorsMoving) {
//             _nextScrollTime += WAIT_TO_SCROLL;
//             return;
//         }

//         // all motors are paused - we're ok to scroll now
//         _currentDisplayScrollPosition += CHARS_TO_SCROLL;

// #if DEBUG
//         Serial.print("SCROLLING TO POSITION: ");
//         Serial.print(_currentDisplayScrollPosition);

//         String displayString = _currentDisplay.substring(
//             _currentDisplayScrollPosition,
//             _currentDisplayScrollPosition + CHARACTER_COUNT);
//         Serial.print(" (");
//         Serial.print(displayString);
//         Serial.println(")");
// #endif

//         setAnimationType(_scrollAnimationType);
//         lerpToCurrentDisplay();
//     }
}

void FlipDisplay::checkForEnable() {
    if (_isDisabled && !areAllMotorsDisabled()) {
        // need to renable the motors
        enable();
    }
}

void FlipDisplay::checkForMotorsMoving() {
    bool areMotorsMoving = false;

    for (int i = START_CHARACTER; i < CHARACTER_COUNT && !areMotorsMoving; i++) {
        switch (_characters[i]->getState()) {
            case FlipDisplayCharacter::FlipDisplayCharacterState::
                HOMING_PAST_BUTTON:
            case FlipDisplayCharacter::FlipDisplayCharacterState::
                HOMING_TO_BUTTON:
            case FlipDisplayCharacter::FlipDisplayCharacterState::
                HOMING_TO_OFFSET:
            case FlipDisplayCharacter::FlipDisplayCharacterState::RUNNING:
                areMotorsMoving = true;
                break;
        }
    }

    if (_areMotorsMoving && !areMotorsMoving) {
#if DEBUG
        Serial.println("MOTORS STOPPED MOVING");
        Serial.print("_currentTime: ");
        Serial.println(_currentTime);
        Serial.print("_minDuration: ");
        Serial.println(_minDuration);
#endif
        // motors stopped moving, if we should be delaying for some period of time, set that now
        _acceptNextCommand = _currentTime + _minDuration;
    }

    _areMotorsMoving = areMotorsMoving;
}

void FlipDisplay::checkForHoming() {
    bool isHoming = false;

    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        switch (_characters[i]->getState()) {
            case FlipDisplayCharacter::FlipDisplayCharacterState::
                HOMING_PAST_BUTTON:
            case FlipDisplayCharacter::FlipDisplayCharacterState::
                HOMING_TO_BUTTON:
            case FlipDisplayCharacter::FlipDisplayCharacterState::
                HOMING_TO_OFFSET:
                // true as long as at least one character is homing
                isHoming = true;
                break;
        }
    }

    if (_isHoming && !isHoming) {
        _isHoming = isHoming;
        _currentDisplay = "";
#if DEBUG_LOOP
        Serial.println("HOMING COMPLETE");
#endif
    }
}

void FlipDisplay::checkForDisable() {
    if (!_isDisabled && areAllMotorsDisabled()) {
        // need to disable the motors
        disable();
    }
}

bool FlipDisplay::areAllMotorsDisabled() {
    bool allMotorsDisabled = true;

    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        switch (_characters[i]->getState()) {
            case FlipDisplayCharacter::FlipDisplayCharacterState::MOTOR_OFF:
                break;

            default:
                allMotorsDisabled = false;
                break;
        }
    }

    return allMotorsDisabled;
}

void FlipDisplay::lerpToCurrentDisplay() {
    String displayString = getStringToDisplay();
    FlipDisplayLerp lerps[CHARACTER_COUNT];

    // determine the number of steps for each character
    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        char displayChar =
            (displayString.length() > i) ? displayString[i] : ' ';
        int steps = _characters[i]->getStepsToChar(displayChar);

        if (steps > 0 && steps < MIN_STEPS_PER_LERP) {
            steps += STEPS_PER_REVOLUTION;
        }

        lerps[i] = FlipDisplayLerp(steps, FlipDisplayLerp::LerpType::FLAT);

        if (steps > STEPS_PER_REVOLUTION) {
            // the character is telling us it needs to loop
            _characters[i]->allowLoop();
        }
    }

    // mutate the lerps duration and delay based on the animation type
    unsigned long maxDuration =
        FlipDisplayLerp::getMaxDuration(lerps, CHARACTER_COUNT);

    switch (_type) {
        case AnimationType::ARRIVE_TOGETHER:
            for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
                lerps[i].setDelay(maxDuration - lerps[i].getTotalDuration());
            }
            break;

        case AnimationType::SAME_DURATION:
            for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
                lerps[i].setDuration(maxDuration);
            }
            break;

        case AnimationType::CASCADE_LEFT: {
            int delayAmt = 0;
            for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
                lerps[i].setDuration(maxDuration);

                if (lerps[i].getTotalSteps() > 0) {
                    lerps[i].setDelay(delayAmt + maxDuration -
                                      lerps[i].getTotalDuration());
                    delayAmt += (maxDuration / 2);
                }
            }
        } break;

        case AnimationType::CASCADE_RIGHT: {
            int delayAmt = 0;
            for (int i = CHARACTER_COUNT - 1; i >= START_CHARACTER; i--) {
                lerps[i].setDuration(maxDuration);

                if (lerps[i].getTotalSteps() > 0) {
                    lerps[i].setDelay(delayAmt + maxDuration -
                                      lerps[i].getTotalDuration());
                    delayAmt += (maxDuration / 2);
                }
            }
        } break;

        case AnimationType::START_TOGETHER:
        default:
            break;
    }

    // start the lerps
    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        _characters[i]->startLerp(lerps[i]);
    }

    if (SCROLLING_ENABLED) {
        if (_currentDisplayWord + 1 < _currentDisplayWordCount) {
            _nextScrollTime = _currentTime + maxDuration + WAIT_TO_SCROLL;
        } else {
             _nextScrollTime = 0;
        }
    }
    // if (SCROLLING_ENABLED) {
    //     // Determine if we need to scroll
    //     if (_currentDisplay.length() >
    //         _currentDisplayScrollPosition + CHARACTER_COUNT) {
    //         _nextScrollTime = _currentTime + maxDuration + WAIT_TO_SCROLL;
    //     } else {
    //         _nextScrollTime = 0;
    //     }
    // }
}

void FlipDisplay::calculateWords() {
    String currentWord = "";
    int wordCount = 0;
    int lastSpace = 0;

    for (int i = 0; i < _currentDisplay.length(); i++) {
        if (_currentDisplay[i] == ' ') {
            if (currentWord.length() > CHARACTER_COUNT) {
                // we've surpassed what we can display, chunk off the last bit
                String word = currentWord.substring(0, lastSpace);
                currentWord = currentWord.substring(lastSpace + 1) + _currentDisplay[i];

                _currentDisplayWords[wordCount] = word;
                wordCount++;
            } else {
                currentWord = currentWord + _currentDisplay[i];
            }

            lastSpace = currentWord.length() - 1;
        } else {
            currentWord = currentWord + _currentDisplay[i];
        }
    }

    // we got to the end
    if (lastSpace == 0) {
        // theres just a single word
        currentWord = _currentDisplay;
    } else if (currentWord.length() > CHARACTER_COUNT) {
        // we've surpassed what we can display, chunk off the last bit
        String word = currentWord.substring(0, lastSpace);
        currentWord = currentWord.substring(lastSpace + 1);

        _currentDisplayWords[wordCount] = word;
        wordCount++;
    }
    
    _currentDisplayWords[wordCount] = currentWord;
    wordCount++;

#if DEBUG
    Serial.println("ALL WORDS: ");
    for (int i = 0; i < wordCount; i++) {
        Serial.println(_currentDisplayWords[i]);
    }
#endif

    _currentDisplayWordCount = wordCount;
    _currentDisplayWord = 0;
}

String FlipDisplay::getWord(int wordIndex) {
    return _currentDisplayWords[wordIndex];
}

String FlipDisplay::getStringToDisplay() {
    String displayString = _currentDisplay;

    // first get a single word if necessary
    if (_currentDisplayWordCount > 1) {
        displayString = getWord(_currentDisplayWord);
    }
    
    // next get the position based on the scroll amount
    displayString = displayString.substring(
        _currentDisplayScrollPosition,
        _currentDisplayScrollPosition + CHARACTER_COUNT);

    return displayString;
}