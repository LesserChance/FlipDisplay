// FlipDisplayCharacter.cpp

#include "FlipDisplayCharacter.h"

/**
 * PUBLIC
 */

FlipDisplayCharacter::FlipDisplayCharacterState FlipDisplayCharacter::run() {
    _currentTime = micros();
    // _enablePinValue = LOW;

    stepOnTime();

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
            detectLoop();
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
            // so for now, we wont disable it
            // _enablePinValue = HIGH;
            break;
    }

    _prevButtonPinValue = _buttonPinValue;
    return _state;
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

    // todo current Position may not be at 0 for the cvhar - so we need ot adjust basede on that


    return steps;
}

void FlipDisplayCharacter::startLerp(FlipDisplayLerp lerp) {
    if (_state != FlipDisplayCharacterState::PAUSED && _state != FlipDisplayCharacterState::MOTOR_OFF) {
        // we can only start a lerp if we are paused
#if DEBUG
        Serial.println("Tried to lerp unpaused character");
        Serial.print("CHARACTER: ");
        Serial.println(_characterIndex);
        Serial.print("STATE: ");
        Serial.println(_state);
#endif
        return;
    }

#if DEBUG
    // lerp.debug();
#endif

    int steps = lerp.getTotalSteps();

    _lerp = lerp;
    _state = FlipDisplayCharacterState::RUNNING;
    _nextStepTime = 0;

    setTargetPosition(steps);
}

void FlipDisplayCharacter::setTargetPosition(int steps) {
    _targetPosition = (_currentPosition + steps) % STEPS_PER_REVOLUTION;
}

void FlipDisplayCharacter::home() {
    _debounceStart = 0;

    if (_buttonPinValue) {
        // currently on the button, so need to get past it
        _state = FlipDisplayCharacterState::HOMING_PAST_BUTTON;
    } else {
        // currently past the button, so need to get to it
        _state = FlipDisplayCharacterState::HOMING_TO_BUTTON;
    }
    
    _nextStepTime = 0;
}

void FlipDisplayCharacter::pause() {
    _state = FlipDisplayCharacterState::PAUSED;
    _targetPosition = -1;
    _pausedTime = micros();
}

uint8_t FlipDisplayCharacter::getStepPinValue() {
    return _stepPinValue;
}

void FlipDisplayCharacter::debounceButtonState(uint8_t value) {
    if (_buttonPinValue != value) {
        if (_debounceStart > 0 && _debounceStart + DEBOUNCE_DURATION <= _currentTime) {
           setButtonState(value);
            _debounceStart = 0;
        } else if (_debounceStart == 0) {
            _debounceStart = _currentTime;
        }
    }
}
void FlipDisplayCharacter::setButtonState(uint8_t value) {
#if DEBUG
        Serial.print("CHAR ");
        Serial.print(_characterIndex);
        Serial.print(" => ");
        Serial.println(value);
#endif

    _buttonPinValue = value;
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
    if (_state == FlipDisplayCharacterState::PAUSED) {
        _state = FlipDisplayCharacterState::RUNNING;
    }

    setTargetPosition(steps);
}


void FlipDisplayCharacter::runToButtonState(uint8_t targetButtonState) {
    if (_buttonPinValue != targetButtonState) {
        // keep stepping
        stepOnTime();
    } else {
        // we've hit the target state
        switch (_state) {
            case FlipDisplayCharacterState::HOMING_PAST_BUTTON:
                // we just got past the button, now get to the button
                _state = FlipDisplayCharacterState::HOMING_TO_BUTTON;
                break;

            case FlipDisplayCharacterState::HOMING_TO_BUTTON:
                // we just got to the button, now get to the offset
                _state = FlipDisplayCharacterState::HOMING_TO_OFFSET;
                _currentPosition = 0;
                step(_startOffset);
                break;
        }
    }
}

void FlipDisplayCharacter::detectLoop() {
    if (_buttonPinValue == 1 && _prevButtonPinValue != _buttonPinValue) {
#if DEBUG
        Serial.print("CHAR ");
        Serial.print(_characterIndex);
        Serial.print(" LOOPED (assumed: ");
        Serial.print(_currentPosition);
        Serial.print(", actual: ");
        Serial.print((STEPS_PER_REVOLUTION - _startOffset));
        Serial.println(")");
#endif

        // reset the current position
        _currentPosition = (STEPS_PER_REVOLUTION - _startOffset);
    }
}

void FlipDisplayCharacter::stepOnTime() {
    if (_state == FlipDisplayCharacterState::PAUSED || _state == FlipDisplayCharacterState::MOTOR_OFF) {
         // this state should not be stepping
         return;
    }
    
    if (_targetPosition != -1 && _currentPosition == _targetPosition) {
        pause();
    } else if (_currentTime >= _nextStepTime) {
        // we have waited at least the right amount of time, go ahead and step
        _phase = !_phase;
        _stepPinValue = _phase;
        _currentPosition = (_currentPosition + 1) % STEPS_PER_REVOLUTION;
        calculateNextStepTime();
    }
}

void FlipDisplayCharacter::calculateNextStepTime() {
    switch (_state) {
        case FlipDisplayCharacterState::RUNNING:
            if (_lerp.isComplete()) {
                // we should be at our character, but we're not, keep going at the last speed
                // this might happen if my lerp curves arent right or a loop reset the position
                _nextStepTime = _currentTime + _lerp.getTimeToNextStep();
            } else {
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

FlipDisplayCharacter::FlipDisplayCharacter(int characterIndex, uint8_t startOffset) {
    _state = FlipDisplayCharacterState::MOTOR_OFF;
    _phase = LOW;

    _characterIndex = characterIndex;
    _currentPosition = 0;
    _targetPosition = -1;
    _debounceStart = 0;

    _startOffset = startOffset;
    _nextStepTime = 0;
    
    _stepPinValue = LOW;
    _enablePinValue = HIGH;
    _buttonPinValue = LOW;
    _prevButtonPinValue = LOW;
}