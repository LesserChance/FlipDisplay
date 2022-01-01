#ifndef FlipDisplayServer_H
#define FlipDisplayServer_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>

#include "Config.h"
#include "FlipDisplay.h"

class FlipDisplayServer {
   public:
    FlipDisplayServer();
    FlipDisplayServer(FlipDisplay display);

    void setup();
    
   private:
    FlipDisplay _display;

    //Variables to save values from HTML form
    String _ssid;
    String _pass;

    // File paths to save input values permanently
    const char* _ssidPath = "/ssid.txt";
    const char* _passPath = "/pass.txt";
    
    void initSPIFFS();

    String readFile(fs::FS &fs, const char * path);
    void writeFile(fs::FS &fs, const char * path, const char * message);
    void deleteFile(fs::FS &fs, const char * path);
    
    bool initWiFi();
    void initSoftAP();
    void initServer();
    void setupRouting();
    void resetSSID();

    void setWifi(AsyncWebServerRequest *request);
    void enableDisplayMotors(AsyncWebServerRequest *request);
    void disableDisplayMotors(AsyncWebServerRequest *request);
    void homeDisplay(AsyncWebServerRequest *request);
    void setDisplay(AsyncWebServerRequest *request);
    void stepCharacter(AsyncWebServerRequest *request);
    void randomWord(AsyncWebServerRequest *request);

};

#endif