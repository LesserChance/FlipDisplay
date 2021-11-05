#ifndef FLIPDISPLAY_TIME_H
#define FLIPDISPLAY_TIME_H

#include "Arduino.h"
#include "time.h"

#include "FlipDisplay_Config.h"

void setTimeByNTP();
String getTime();

#endif