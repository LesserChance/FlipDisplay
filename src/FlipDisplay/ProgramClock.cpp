
#include "ProgramClock.h"

ProgramClock::ProgramClock() {}

ProgramClock::ProgramClock(FlipDisplay *display) { _display = display; }

void ProgramClock::setupProgram() { setTimeByNTP(); }

void ProgramClock::run(bool buttonOne, bool programSwitch) {
    unsigned long _currentTime = millis();

    if (buttonOne) {
        String word = words[random(0, WORD_COUNT)];
        _display->setDisplay(word, MICROSECONDS * 30, true);
        _lastRunTime = _currentTime + 15000;
    }

    if (programSwitch || _currentTime >= _lastRunTime + 10000) {
        _resetCountdown--;
        if (_resetCountdown == 0) {
            // reset occasionally to make sure we're handling any drift
            setTimeByNTP();
            _resetCountdown = RESET_FREQUENCY;
        }

        _display->setDisplay(getTime());
        _lastRunTime = _currentTime;
    }
}

void ProgramClock::setTimeByNTP() {
#if DEBUG
        Serial.println("CLOCK: Reset clock by NTP");
#endif
    configTime(GMT_OFFSET * 3600, 0, "pool.ntp.org");
}

String ProgramClock::getTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
#if DEBUG_RESPONSES
        Serial.println("CLOCK: could not obtain time");
#endif
        return "";
    }

    int time = timeinfo.tm_hour;

    // uncomment to debug more frequent changes
    // Serial.print("min: ");
    // Serial.print(timeinfo.tm_min);
    // Serial.print(" => ");
    // Serial.println(timeinfo.tm_min % 12);
    // time = timeinfo.tm_min % 12;

    switch (time) {
        case 1:
            return "one am";
            break;
        case 13:
            return "one pm";
            break;
        case 2:
            return "two am";
            break;
        case 14:
            return "two pm";
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
            return "six am";
            break;
        case 18:
            return "six pm";
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
            return "ten am";
            break;
        case 22:
            return "ten pm";
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
