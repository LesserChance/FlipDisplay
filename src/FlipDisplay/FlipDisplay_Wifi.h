#ifndef FLIPDISPLAY_WIFI_H
#define FLIPDISPLAY_WIFI_H

#include "Arduino.h"
#include "WiFi.h"
#include "HTTPClient.h"

#include "FlipDisplay_CharacterControl.h"

void connectWifi();

void requestWord();

#endif