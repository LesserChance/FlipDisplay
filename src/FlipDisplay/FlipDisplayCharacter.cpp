// FlipDisplayCharacter.cpp

#include "FlipDisplayCharacter.h"

/*************************************
 * CONSTRUCTOR
 *************************************/

FlipDisplayCharacter::FlipDisplayCharacter(int characterIndex,
                                           int startOffset) {
    _state = FlipDisplayCharacterState::MOTOR_OFF;
    _phase = LOW;

    _characterIndex = characterIndex;
    _currentPosition = 0;
    _targetPosition = -1;
    _debounceStart = 0;

    _startOffset = startOffset;
    _nextStepTime = 0;

    _stepPinValue = LOW;
    _buttonPinValue = LOW;
    _prevButtonPinValue = LOW;
}

/*************************************
 * PRIVATE
 *************************************/

FlipDisplayCharacter::FlipDisplayCharacterState FlipDisplayCharacter::run() {
    _currentTime = micros();

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
                // since we're paused, that means we've moved the appropriate
                // distance
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
            break;
    }

    _prevButtonPinValue = _buttonPinValue;
    return _state;
}

void FlipDisplayCharacter::startLerp(FlipDisplayLerp lerp) {
    if (_state != FlipDisplayCharacterState::PAUSED &&
        _state != FlipDisplayCharacterState::MOTOR_OFF) {
        // we can only start a lerp if we are paused
#if DEBUG_LERP
        Serial.println("Tried to lerp unpaused character");
        Serial.print("CHARACTER: ");
        Serial.println(_characterIndex);
        Serial.print("STATE: ");
        Serial.println(_state);
#endif
        return;
    }

#if DEBUG_LERP
    lerp.debug();
#endif

    int steps = lerp.getTotalSteps();

    _lerp = lerp;
    _state = FlipDisplayCharacterState::RUNNING;
    _nextStepTime = 0;

    setTargetPosition(steps);
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

void FlipDisplayCharacter::step(int steps) {
    if (_state == FlipDisplayCharacterState::PAUSED ||
        _state == FlipDisplayCharacterState::MOTOR_OFF) {
        _state = FlipDisplayCharacterState::RUNNING;
    }

    setTargetPosition(steps);
}

void FlipDisplayCharacter::allowLoop() { _allowLoop = true; }

void FlipDisplayCharacter::debounceButtonState(uint8_t value) {
    if (_buttonPinValue != value) {
        if (_debounceStart > 0 &&
            _debounceStart + DEBOUNCE_DURATION <= _currentTime) {
            setButtonState(value);
            _debounceStart = 0;
        } else if (_debounceStart == 0) {
            _debounceStart = _currentTime;
        }
    }
}

void FlipDisplayCharacter::setButtonState(uint8_t value) {
#if DEBUG_LOOP
    Serial.print("CHAR ");
    Serial.print(_characterIndex);
    Serial.print(" => ");
    Serial.println(value);
#endif

    _buttonPinValue = value;
}

FlipDisplayCharacter::FlipDisplayCharacterState
FlipDisplayCharacter::getState() {
    return _state;
}

uint8_t FlipDisplayCharacter::getStepPinValue() { return _stepPinValue; }

int FlipDisplayCharacter::getStepsToChar(char targetChar) {
    if (targetChar == '1') {
        targetChar = 'I';
    }
    if (targetChar == '0') {
        targetChar = 'O';
    }

    int currentIndex = _currentPosition / STEPS_PER_CHARACTER;
    int targetIndex = getCharPosition(targetChar);
    int steps = ((POSSIBLE_CHARACTER_VALUES + (targetIndex - currentIndex)) %
                 POSSIBLE_CHARACTER_VALUES) *
                STEPS_PER_CHARACTER;

    if (targetIndex > currentIndex &&
        _state == FlipDisplayCharacterState::MOTOR_OFF) {
        // we need to ensure we do a full loop, since when the mototr goes off
        // it could roll back a bit this will effectively re-home every
        // character
        steps = steps + STEPS_PER_REVOLUTION;
    }

    return steps;
}

int FlipDisplayCharacter::getCurrentOffsetFromButton() {
    return _currentPosition + _startOffset;
}

/*************************************
 * PUBLIC
 *************************************/

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
#if DEBUG_LOOP
        Serial.print("CHAR ");
        Serial.print(_characterIndex);
        Serial.print(" LOOPED (assumed: ");
        Serial.print(_currentPosition);
        Serial.print(", actual: ");
        Serial.print((STEPS_PER_REVOLUTION - _startOffset));
        Serial.print(", with offset: ");
        Serial.print(
            (STEPS_PER_REVOLUTION - _startOffset + LOOP_REVERSE_OFFSET));
        Serial.println(")");
#endif

        // reset the current position
        _currentPosition =
            (STEPS_PER_REVOLUTION - _startOffset + LOOP_REVERSE_OFFSET);
    }
}

void FlipDisplayCharacter::stepOnTime() {
    if (_state == FlipDisplayCharacterState::PAUSED ||
        _state == FlipDisplayCharacterState::MOTOR_OFF) {
        // this state should not be stepping
        return;
    }

    bool checkForStep = true;

    if (_targetPosition != -1 && _currentPosition == _targetPosition) {
        // we hit the target position
        checkForStep = false;

        if (_allowLoop) {
            // we're supposed to keep going but stop next time we hit it
            if (_currentTime >= _nextStepTime) {
                // since this could happen mutliple times before we actually
                // step, we need to wait until we know a step will happen
                _allowLoop = false;
                checkForStep = true;
            }
        } else {
            // we are supposed to stop here
#if DEBUG_LOOP
            Serial.print("CHAR ");
            Serial.print(_characterIndex);
            Serial.print(" ARRIVED, _currentPosition: ");
            Serial.print(_currentPosition);
            Serial.print(", _targetPosition: ");
            Serial.print(_targetPosition);
            Serial.println(")");
#endif
            pause();
        }
    }

    if (checkForStep && _currentTime >= _nextStepTime) {
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
                // we should be at our character, but we're not, keep going at
                // the last speed this might happen if my lerp curves arent
                // right or a loop reset the position
                _nextStepTime = _currentTime + _lerp.getLastStepDuration();
            } else {
                _nextStepTime = _currentTime + _lerp.getTimeToNextStep();
            }
            break;

        case FlipDisplayCharacterState::HOMING_PAST_BUTTON:
        case FlipDisplayCharacterState::HOMING_TO_BUTTON:
        case FlipDisplayCharacterState::HOMING_TO_OFFSET:
            // just move at the standard rate
            _nextStepTime = _currentTime + HOMING_STEP_DURATION;
            break;
    }
}

void FlipDisplayCharacter::setTargetPosition(int steps) {
    _targetPosition = (_currentPosition + steps) % STEPS_PER_REVOLUTION;
}

void FlipDisplayCharacter::setCurrentPosition(int currentPosition) {
    _currentPosition = currentPosition;
}

int FlipDisplayCharacter::getCharPosition(char targetChar) {
    for (int i = 0; i < POSSIBLE_CHARACTER_VALUES; i++) {
        if (_characters[i] == targetChar) {
            return i;
        }
    }
    return -1;
}