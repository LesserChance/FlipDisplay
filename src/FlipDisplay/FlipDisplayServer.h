#ifndef FlipDisplayServer_H
#define FlipDisplayServer_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <JSON.h>
#include <JSONVar.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "FlipDisplay.h"
#include "FlipDisplayConfig.h"

extern AsyncWebServer webserver;

class FlipDisplayServer {
   public:
    FlipDisplayServer();
    FlipDisplayServer(FlipDisplay *display);

    void setupServer();

   private:
    FlipDisplay *_display;
    String _ssid;
    String _pass;

    void initSPIFFS();
    bool initWiFi();
    void initSoftAP();
    void initServer();
    void setupRouting();
    
    String processor(const String &var);

    void setWifi(AsyncWebServerRequest *request);
    void enableDisplayMotors(AsyncWebServerRequest *request);
    void disableDisplayMotors(AsyncWebServerRequest *request);
    void setDisplayPower(bool powerOn, AsyncWebServerRequest *request);
    void homeDisplay(AsyncWebServerRequest *request);
    void setDisplay(AsyncWebServerRequest *request);
    void stepCharacter(AsyncWebServerRequest *request);
    void randomWord(AsyncWebServerRequest *request);

    void updateSonosAccess(AsyncWebServerRequest *request);
    void updateSonosSettings(AsyncWebServerRequest *request);
    void displaySonosGroups(AsyncWebServerRequest *request);
};

#endif