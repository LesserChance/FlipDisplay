// FlipDisplayLerp.cpp

#include "FlipDisplayLerp.h"

unsigned long FlipDisplayLerp::getTimeToNextStep() {
    if (_totalDuration == 0) {
        // there were no steps, this is an error scenario
        return 0;
    }

    unsigned long stepDuration = 0;

    if (_pointer == 0 && _delay > 0) {
        // the first step should be the delay
        unsigned long returnDelay = _delay;
        _delay = 0;
        return returnDelay;
    }

    switch (_type) {
        case LerpType::LINEAR_SPEED_UP:
            stepDuration = getStepDurationSpeedUp(_pointer);
            break;

        case LerpType::LINEAR_SLOW_DOWN:
            stepDuration = getStepDurationSlowDown(_pointer);
            break;

        case LerpType::RAMP_UP_MAX:
            stepDuration = getStepDurationRampUpMax(_pointer);
            break;

        case LerpType::RAMP_DOWN_MAX:
            stepDuration = getStepDurationRampDownMax(_pointer);
            break;

        case LerpType::FLAT:
        default:
            stepDuration = getStepDurationFlat();
            break;
    }
    
    _pointer++;

    _lastStepDuration = stepDuration;
    return stepDuration;
}

int FlipDisplayLerp::getTotalSteps() {
    return _totalSteps;
}

unsigned long FlipDisplayLerp::getTotalDuration() {
    return _totalDuration;
}

unsigned long FlipDisplayLerp::getLastStepDuration() {
    return _lastStepDuration;
}

bool FlipDisplayLerp::isComplete() {
    return  (_pointer >= _totalSteps);
}

void FlipDisplayLerp::setDelay(unsigned long delay) {
    if (_totalSteps == 0) {
        return;
    }
    
    _delay = delay;
}

void FlipDisplayLerp::debug() {
    Serial.println("LERP:");
    // Serial.print("_type:");
    // Serial.println(_type);
    Serial.print("_totalDuration:");
    Serial.println(_totalDuration);
    Serial.print("_totalSteps:");
    Serial.println(_totalSteps);
    // Serial.print("_pointer:");
    // Serial.println(_pointer);
    // Serial.print("_scale:");
    // Serial.println(_scale);
}

void FlipDisplayLerp::setDuration(unsigned long duration) {
    if (_totalSteps == 0) {
        return;
    }

    _totalDuration = duration;
    _scale = 0;

    if (_type != LerpType::FLAT) {
        // What percent can we move the min and max by to not go below the min threshold
        unsigned long flatDuration = getStepDurationFlat();
        _scale = (double) (flatDuration - MIN_STEP_DURATION) * 100 / flatDuration;
    }
}

unsigned long FlipDisplayLerp::getStepDurationSpeedUp(int step) {
    unsigned long flatDuration = getStepDurationFlat();

    // scale steps (+X% -> -X%)
    double scale = (double) step / _totalSteps;
    double mapped = map(scale * 100, 0, 100, 100 + _scale, 100 - _scale);

    return (flatDuration * (mapped / 100));
}

unsigned long FlipDisplayLerp::getStepDurationSlowDown(int step) {
    unsigned long flatDuration = getStepDurationFlat();

    // scale steps (-X% -> +X%)
    double scale = (double) step / _totalSteps;
    double mapped = map(scale * 100, 0, 100, 100 - _scale, 100 + _scale);

    return (flatDuration * (mapped / 100));
}

unsigned long FlipDisplayLerp::getStepDurationRampDownMax(int step) {
    unsigned long flatDuration = getStepDurationFlat();

    // scale steps (-X% -> +X% -> -X%)
    double scale = (double) abs(step - (_totalSteps / 2)) / _totalSteps;
    double mapped = map(scale * 100, 50, 0, 100 - _scale, 100 + _scale);

    return (flatDuration * (mapped / 100));
}

unsigned long FlipDisplayLerp::getStepDurationRampUpMax(int step) {
    unsigned long flatDuration = getStepDurationFlat();

    // scale steps (+X% -> -X% -> +X%)
    double scale = (double) abs(step - (_totalSteps / 2)) / _totalSteps;
    double mapped = map(scale * 100, 50, 0, 100 + _scale, 100 - _scale);

    return (flatDuration * (mapped / 100));
}

unsigned long FlipDisplayLerp::getStepDurationFlat() {
    if (_totalSteps == 0) {
        return 0;
    }

    return (unsigned long) _totalDuration / _totalSteps;
}

unsigned long FlipDisplayLerp::getAverageStepDuration() {
    return getStepDurationFlat();
}

void FlipDisplayLerp::addLoop() {
    _totalSteps = _totalSteps + STEPS_PER_REVOLUTION;
}

unsigned long FlipDisplayLerp::getMaxDuration(FlipDisplayLerp* lerps, int lerpCount) {
    unsigned long maxDuration = 0;

    for (int i = 0; i < lerpCount; i++){
        maxDuration = max(lerps[i].getTotalDuration(), maxDuration);
    }

    return maxDuration;
}

FlipDisplayLerp::FlipDisplayLerp(int steps, LerpType type, unsigned long duration) {
    _type = type;
    _totalSteps = steps;
    _pointer = 0;
    _delay = 0;
    _lastStepDuration = 0;

    if (steps == 0) {
        // this is an error scenario, we probably just shouldnt set this lerp
        _totalDuration = 0;
        _scale = 0;
        return;
    }
    
    if (duration == 0) {
        // just determine based on the average step duration
        duration = (unsigned long) AVG_STEP_DURATION * steps;
    }

    setDuration(duration);
}