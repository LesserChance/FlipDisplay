// FlipDisplayCharacter.cpp

#include "FlipDisplayCharacter.h"

/**
 * PUBLIC
 */

void FlipDisplayCharacter::run() {
    _currentTime = micros();
    _enablePinValue = LOW;

    switch (_state) {
        case FlipDisplayCharacterState::HOMING_PAST_BUTTON:
            runToButtonState(LOW);
            break;

        case FlipDisplayCharacterState::HOMING_TO_BUTTON:
            runToButtonState(HIGH);
            break;

        case FlipDisplayCharacterState::HOMING_TO_OFFSET:
            stepOnTime();

            if (_state == FlipDisplayCharacterState::PAUSED) {
                // since we're paused, that means we've moved the appropriate distance
                setCurrentPosition(0);
            }
            break;

        case FlipDisplayCharacterState::RUNNING:
            stepOnTime();
            break;

        case FlipDisplayCharacterState::PAUSED:
            if (_currentTime >= _pausedTime + PAUSE_ENABLED_DURATION) {
                _state = FlipDisplayCharacterState::MOTOR_OFF;
            }
            break;

        case FlipDisplayCharacterState::MOTOR_OFF:
        default:
            // it seems as though the motor can roll back when we disable it, which then loses its position
            // do for now, we wont disable it
            // _enablePinValue = HIGH;
            break;
    }
}

int FlipDisplayCharacter::getStepsToChar(char targetChar) {
    if (targetChar == '1') {
        targetChar = 'I';
    }
    if (targetChar == '0') {
        targetChar = 'O';
    }

    int currentIndex = _currentPosition / STEPS_PER_CHARACTER;
    int targetIndex = getCharPosition(targetChar);
    int steps = ((POSSIBLE_CHARACTER_VALUES + (targetIndex - currentIndex)) % POSSIBLE_CHARACTER_VALUES) * STEPS_PER_CHARACTER;

    return steps;
}

void FlipDisplayCharacter::startLerp(FlipDisplayLerp lerp) {
    if (_state != FlipDisplayCharacterState::PAUSED && _state != FlipDisplayCharacterState::MOTOR_OFF) {
        // we can only start a lerp if we are paused
        Serial.println("Tried to lerp unpaused character");
        return;
    }
    // lerp.debug();

    int steps = lerp.getTotalSteps();

    _lerp = lerp;
    _state = FlipDisplayCharacterState::RUNNING;
    _nextStepTime = _currentTime;

    setTargetPosition(steps);
}

void FlipDisplayCharacter::setTargetPosition(int steps) {
    _targetPosition = (_currentPosition + steps) % STEPS_PER_REVOLUTION;
}

void FlipDisplayCharacter::home() {
    _debounceStart = 0;

    if (digitalRead(_buttonPin)) {
        _state = FlipDisplayCharacterState::HOMING_PAST_BUTTON;
    } else {
        _state = FlipDisplayCharacterState::HOMING_TO_BUTTON;
    }
}

void FlipDisplayCharacter::pause() {
    _state = FlipDisplayCharacterState::PAUSED;
    _targetPosition = -1;
    _pausedTime = micros();
}

uint8_t FlipDisplayCharacter::getStepPinValue() {
    return _stepPinValue;
}

uint8_t FlipDisplayCharacter::getEnablePinValue() {
    return _enablePinValue;
}

/**
 * PRIVATE
 */

void FlipDisplayCharacter::setCurrentPosition(int currentPosition) {
    _currentPosition = currentPosition;
}

void FlipDisplayCharacter::step(int steps) {
    //todo: steps should always be even since it takes two steps to move
    if (_state == FlipDisplayCharacterState::PAUSED) {
        _state = FlipDisplayCharacterState::RUNNING;
    }

    setTargetPosition(steps);
}


void FlipDisplayCharacter::runToButtonState(uint8_t targetButtonState) {
    uint8_t buttonState = digitalRead(_buttonPin);

    if (buttonState != targetButtonState) {
        // keep stepping
        stepOnTime();
    } else {
        // we've hit the target state
        if (_debounceStart > 0 && _debounceStart + DEBOUNCE_DURATION <= _currentTime) {
            // we're properly debounced
             switch (_state) {
                case FlipDisplayCharacterState::HOMING_PAST_BUTTON:
                    // we just got past the button, now get to the button
                    _state = FlipDisplayCharacterState::HOMING_TO_BUTTON;
                    _debounceStart = 0;
                    break;

                case FlipDisplayCharacterState::HOMING_TO_BUTTON:
                    // we just got to the button, now get to the offset
                    _state = FlipDisplayCharacterState::HOMING_TO_OFFSET;
                    _currentPosition = 0;
                    step(_startOffset);
                    break;
            }
        } else {
            // we need to debounce
            if (_debounceStart == 0) {
                _debounceStart = _currentTime;
            }

            stepOnTime();
        }
    }
}

void FlipDisplayCharacter::stepOnTime() {
    if (_state == FlipDisplayCharacterState::PAUSED) {
         // this state should not be stepping
         return;
    }
    
    if (_targetPosition != -1 && _currentPosition == _targetPosition) {
        // todo: the lerp should control this stopping at the right point?
        pause();
    } else if (_currentTime >= _nextStepTime) {
        // we are past the step point, go ahead and move to the next phase
        _phase = !_phase;
        _stepPinValue = _phase;
        
        switch (_state) {
            case FlipDisplayCharacterState::RUNNING:
                if (_lerp.isComplete()) {
                    // we should be at our character, but we're not, keep going at the last speed
                    // this might happen if my lerp curves arent right
                    Serial.println("STILL LOOOPING?");
                    Serial.println(_currentPosition);
                    _nextStepTime = _currentTime + _lerp.getTimeToNextStep();
                    // pause();
                } else {
                    // Serial.print(_currentTime);
                    // Serial.println(": step");
                    _nextStepTime = _currentTime + _lerp.getTimeToNextStep();
                }
                break;

            case FlipDisplayCharacterState::HOMING_PAST_BUTTON:
            case FlipDisplayCharacterState::HOMING_TO_BUTTON:
            case FlipDisplayCharacterState::HOMING_TO_OFFSET:
                // just move at the standard rate
                _nextStepTime = _currentTime + FlipDisplayLerp::C_HOMING_STEP_DURATION;
                break;
        }

        _currentPosition = (_currentPosition + 1) % STEPS_PER_REVOLUTION;
    }
}

int FlipDisplayCharacter::getCharPosition(char targetChar) {
    for (int i = 0; i < POSSIBLE_CHARACTER_VALUES; i++) {
        if (_characters[i] == targetChar) {
            return i;
        }
    }
    return -1;
}

/**
 * CONSTRUCTOR
 */

FlipDisplayCharacter::FlipDisplayCharacter(uint8_t buttonPin, uint8_t startOffset) {
    _state = FlipDisplayCharacterState::MOTOR_OFF;
    _phase = LOW;

    _currentPosition = 0;
    _targetPosition = -1;
    _debounceStart = 0;

    _buttonPin = buttonPin;

    _startOffset = startOffset;

    _nextStepTime = 0;
    
    _stepPinValue = LOW;
    _enablePinValue = HIGH;

    pinMode(_buttonPin, INPUT);
}