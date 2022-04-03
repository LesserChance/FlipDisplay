#ifndef ProgramClock_H
#define ProgramClock_H

#include <Arduino.h>
#include <time.h>

#include "FlipDisplay.h"

const int GMT_OFFSET = -4;
const int RESET_FREQUENCY = 80;

class ProgramClock {
   public:
    ProgramClock();
    ProgramClock(FlipDisplay *display);

    void setupProgram();
    void run(bool buttonOne, bool programSwitch);

   private:
    int _resetCountdown = RESET_FREQUENCY;
    FlipDisplay *_display;
    unsigned long _lastRunTime = 0;

    void setTimeByNTP();
    String getTime();
};

#endif