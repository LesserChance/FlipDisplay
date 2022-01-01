
#include "ProgramClock.h"

ProgramClock::ProgramClock() {}

ProgramClock::ProgramClock(FlipDisplay display) {
    _display = display;
}

void ProgramClock::setup() {
    setTimeByNTP();
}

void ProgramClock::run(bool buttonOne, bool buttonTwo) {
    unsigned long _currentTime = millis();

    if (_currentTime >= _lastRunTime + 10000) {
        _display.setDisplay(getTime());
        _lastRunTime = _currentTime;
    }
}

void ProgramClock::setTimeByNTP() {
    configTime(GMT_OFFSET * 3600, 0, "pool.ntp.org");
}

String ProgramClock::getTime() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("CLOCK: could not obtain time");
        return "";
    }

    switch (timeinfo.tm_hour) {
        case 1:
        case 13:
            return " one ";
            break;
        case 2:
        case 14:
            return " two ";
            break;
        case 3:
        case 15:
            return "three";
            break;
        case 4:
        case 16:
            return "four";
            break;
        case 5:
        case 17:
            return "five";
            break;
        case 6:
        case 18:
            return " six ";
            break;
        case 7:
        case 19:
            return "seven";
            break;
        case 8:
        case 20:
            return "eight";
            break;
        case 9:
        case 21:
            return "nine";
            break;
        case 10:
        case 22:
            return " ten ";
            break;
        case 11:
        case 23:
            return "eleven";
            break;
        case 0:
        case 12:
            return "twelve";
            break;
        default:
            return "";
            break;
    }
}
