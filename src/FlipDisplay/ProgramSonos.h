#ifndef ProgramSonos_H
#define ProgramSonos_H

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include <JSON.h>
#include <JSONVar.h>
#include <WiFi.h>

#include "FlipDisplay.h"
#include "FlipDisplayConfig.h"
#include "FlipDisplayServer.h"

#define POLL_FREQUENCY 10000

class ProgramSonos {
   public:
    ProgramSonos();
    ProgramSonos(FlipDisplay *display);

    void setupProgram();
    void run(bool buttonOne, bool programSwitch);

   private:
    FlipDisplay *_display;
    unsigned long _lastRunTime = 0;

    String _displayedArtist = "";
    String _currentArtist;

    String _accessToken;
    String _resetToken;
    String _groupId;

    void poll();
    void resetToken();
    void getCurrentlyPlayingArtist();
    void cleanArtistString();
};

#endif