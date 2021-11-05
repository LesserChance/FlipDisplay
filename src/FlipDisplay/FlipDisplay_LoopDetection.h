#ifndef FLIPDISPLAY_LOOPDETECTION_H
#define FLIPDISPLAY_LOOPDETECTION_H

#include "Arduino.h"
#include "FlipDisplay_CharacterControl.h"
#include "FlipDisplay_Config.h"
#include "FlipDisplay_Registers.h"
#include "FlipDisplay_Reset.h"

/**
 * Perform the initial loop detection setup
 */
void setupLoopDetection();

/**
 * The interrupt will always trigger, but only update data if this is attached - attach it
 */
void attachLoopListener(int board, int motor);

/**
 * The interrupt will always trigger, but only update data if this is attached - unattach it
 */
void unattachLoopListener(int board);

/**
 * return if the listener is attached
 */
bool isLoopListenerAttached(int board);

void trigger_motorAtStart0();
void trigger_motorAtStart1();

/**
 * This trigger is an bound to an interrupt pin, it will interrupt currently executing code
 * when a motor that is being monitored hits it starts position
 */
void trigger_motorAtStart(int board);

/**
 * The current movement of this motor will cause it to pass the start position
 * After a certain number of times this occurs, we can expect the position to be slightly off
 * So every once in a while we want to redetect the start position at re-set 0
 */
void reduceLoopCount(int board, int motor, int targetIndex);

/**
 * Store which board/motor we're currently detecting, as well as the character moving towards
 * This should be set to -1 if we do not want to detect anything
 * 
 * return true if we're actually tracking it
 */
bool setDetectingLoop(int board, int motor, int targetIndex);

/**
 * this is triggered when a motor that is being monitored has hit it's start position
 * at this point, we know the motor at 0, so we should reset our offsets and recalculate
 * this allows for slight differences in total steps per cycle
 */
void loopDetected();
void loopDetected2();

#endif
