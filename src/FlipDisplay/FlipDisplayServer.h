#ifndef FlipDisplayServer_h
#define FlipDisplayServer_h

#include "Config.h"
#include "FlipDisplay.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>

void buttonOnePressed();
void buttonTwoPressed();

void setupDisplay();
void loopDisplay();

void initializeServer();
void initializeGPIO();

void connectWifi();
void setupRouting();

void displayForm();
void homeDisplay();
void setDisplay();
void randomWord();

#endif