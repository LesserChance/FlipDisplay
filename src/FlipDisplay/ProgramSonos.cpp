#include "ProgramSonos.h"

ProgramSonos::ProgramSonos() {}

ProgramSonos::ProgramSonos(FlipDisplay *display) { _display = display; }

void ProgramSonos::setup() {
    _accessToken = FlipDisplayConfig::getPersistedValue(
        FlipDisplayConfig::ConfigKey::SONOS_ACCESS_TOKEN);
    _groupId = FlipDisplayConfig::getPersistedValue(
        FlipDisplayConfig::ConfigKey::SONOS_GROUP_ID);
}

void ProgramSonos::run(bool buttonOne, bool buttonTwo) {
    unsigned long _currentTime = millis();

    if (_displayedArtist != _currentArtist) {
        _displayedArtist = _currentArtist;
        _display->setDisplay(_currentArtist);
    }

    if (_currentTime >= _lastRunTime + POLL_FREQUENCY) {
        poll();

        _lastRunTime = _currentTime;
    }
}

void ProgramSonos::resetToken() { Serial.println("reset"); }

void ProgramSonos::poll() {
    if (WiFi.status() == WL_CONNECTED) {
        // first confirm that we're currently playing
        String url = "https://api.ws.sonos.com/control/api/v1/households/" +
                     String(SONOS_HOUSEHOLD_ID) + "/groups/" +
                     String(_groupId) + "/playback";

        HTTPClient http;
        http.begin(url.c_str());

        http.addHeader("Authorization", "Bearer " + String(_accessToken));
        http.addHeader("Content-Type", "application/json");

        int httpCode = http.GET();

        if (httpCode == 0) {
#if DEBUG_RESPONSES
            Serial.println("http failed");
#endif
            return;
        }

        String payload = http.getString();
        JSONVar jsonResponse = JSON.parse(payload);

        if (JSON.typeof(jsonResponse) == "undefined") {
#if DEBUG_RESPONSES
            Serial.println("Parsing input failed");
#endif
            return;
        }

#if DEBUG_RESPONSES
        Serial.print("playback response => ");
        Serial.println(jsonResponse);
#endif

        if (strcmp(jsonResponse["playbackState"], "PLAYBACK_STATE_PLAYING") ==
                0 ||
            strcmp(jsonResponse["playbackState"], "PLAYBACK_STATE_BUFFERING") ==
                0) {
            // currently playing, can get the artist
            getCurrentlyPlayingArtist();
        }

        http.end();
    }
}

void ProgramSonos::getCurrentlyPlayingArtist() {
    String url = "https://api.ws.sonos.com/control/api/v1/households/" +
                 String(SONOS_HOUSEHOLD_ID) + "/groups/" + String(_groupId) +
                 "/playbackMetadata";
    HTTPClient http;

    http.begin(url.c_str());

    http.addHeader("Authorization", "Bearer " + String(_accessToken));
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();

    if (httpCode == 0) {
#if DEBUG_RESPONSES
        Serial.println("http failed");
#endif
        return;
    }

    String payload = http.getString();
    JSONVar jsonResponse = JSON.parse(payload);

    if (JSON.typeof(jsonResponse) == "undefined") {
#if DEBUG_RESPONSES
        Serial.println("Parsing input failed!");
#endif
        return;
    }

#if DEBUG_RESPONSES
    Serial.print("playbackMetadata response => ");
    Serial.println(jsonResponse);
#endif

    _currentArtist = jsonResponse["currentItem"]["track"]["artist"]["name"];

    http.end();
}