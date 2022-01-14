#ifndef ProgramClock_H
#define ProgramClock_H

#include <Arduino.h>
#include <time.h>

#include "FlipDisplay.h"

const int GMT_OFFSET = -5;

class ProgramClock {
   public:
    ProgramClock();
    ProgramClock(FlipDisplay *display);

    void setupProgram();
    void run(bool buttonOne, bool programSwitch);

   private:
    FlipDisplay *_display;
    unsigned long _lastRunTime = 0;

    void setTimeByNTP();
    String getTime();
};

#endif