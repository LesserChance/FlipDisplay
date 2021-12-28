#ifndef FlipDisplayServer_h
#define FlipDisplayServer_h

#include "Config.h"
#include "FlipDisplay.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>
#include <SPIFFS.h>

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
void stepCharacter();
void randomWord();

void enableDisplayMotors();
void disableDisplayMotors();

#endif