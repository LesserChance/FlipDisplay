#ifndef FLIPDISPLAY_QUEUE_H
#define FLIPDISPLAY_QUEUE_H

#include "FlipDisplay_CharacterControl.h"
#include "FlipDisplay_Config.h"
#include "FlipDisplay_Time.h"

void executeQueueInLoop();
void addInstruction(int msDelay, int instruction, String stringArgs[]);
void executeNextInstruction();

#endif
