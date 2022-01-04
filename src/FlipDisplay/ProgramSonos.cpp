#include "ProgramSonos.h"

const char *PLAYING = "PLAYBACK_STATE_PLAYING";
const char *BUFFERING = "PLAYBACK_STATE_BUFFERING";
const char *NEEDS_RESET = "Access Token expired";



ProgramSonos::ProgramSonos() {}

ProgramSonos::ProgramSonos(FlipDisplay *display) { _display = display; }

void ProgramSonos::setupProgram() {
    _accessToken = FlipDisplayConfig::getPersistedValue(
        FlipDisplayConfig::ConfigKey::SONOS_ACCESS_TOKEN);
    _resetToken = FlipDisplayConfig::getPersistedValue(
        FlipDisplayConfig::ConfigKey::SONOS_RESET_TOKEN);
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
        if (_display->canDisplayBeUpdated()) {
            poll();
        }

        _lastRunTime = _currentTime;
    }
}

void ProgramSonos::resetToken() {
    Serial.println("reset");
    String url = "https://api.sonos.com/login/v3/oauth/access";

    HTTPClient http;
    http.begin(url.c_str());

    http.addHeader("Authorization", "Basic {" + String(SONOS_API_AUTH_B64) + "}");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded;charset=utf-8");

    String postPayload = "grant_type=refresh_token&refresh_token=" + _resetToken;

    int httpCode = http.sendRequest("POST", postPayload);

    if (httpCode == 0) {
        Serial.println("http failed");
        return;
    }

    String payload = http.getString();
    JSONVar jsonResponse = JSON.parse(payload);

    if (JSON.typeof(jsonResponse) == "undefined") {
        Serial.println("Parsing input failed");
        http.end();
        return;
    }

    if (jsonResponse.hasOwnProperty("fault")) {
        Serial.print("reset fault => ");
        Serial.println(jsonResponse["fault"]);
        http.end();
        return;
    }

#if DEBUG_RESPONSES
    Serial.print("reset response => ");
    Serial.println(jsonResponse);
#endif


    // todo: need to store this as a new acces token

    http.end();
}

void ProgramSonos::poll() {
    // first confirm that we're currently playing
    String url = "https://api.ws.sonos.com/control/api/v1/households/" +
                 String(SONOS_HOUSEHOLD_ID) + "/groups/" + String(_groupId) +
                 "/playback";

    HTTPClient http;
    http.begin(url.c_str());

    http.addHeader("Authorization", "Bearer " + String(_accessToken));
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.sendRequest("GET");

    if (httpCode == 0) {
        Serial.println("http failed");
        return;
    }

    String payload = http.getString();
    JSONVar jsonResponse = JSON.parse(payload);

    if (JSON.typeof(jsonResponse) == "undefined") {
        Serial.println("Parsing input failed");
        http.end();
        return;
    }

    if (jsonResponse.hasOwnProperty("fault")) {
        Serial.print("playback fault => ");
        Serial.println(jsonResponse["fault"]);
        http.end();

        if (strcmp(jsonResponse["fault"]["faultstring"], NEEDS_RESET) == 0) {
            resetToken();
        }
        return;
    }

#if DEBUG_RESPONSES
    Serial.print("playback response => ");
    Serial.println(jsonResponse);
#endif

    if (strcmp(jsonResponse["playbackState"], PLAYING) == 0 ||
        strcmp(jsonResponse["playbackState"], BUFFERING) == 0) {
        // currently playing, can get the artist
        getCurrentlyPlayingArtist();
    }

    http.end();
}

void ProgramSonos::getCurrentlyPlayingArtist() {
    String url = "https://api.ws.sonos.com/control/api/v1/households/" +
                 String(SONOS_HOUSEHOLD_ID) + "/groups/" + String(_groupId) +
                 "/playbackMetadata";
    HTTPClient http;

    http.begin(url.c_str());

    http.addHeader("Authorization", "Bearer " + String(_accessToken));
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.sendRequest("GET");

    if (httpCode == 0) {
        Serial.println("http failed");
        return;
    }

    String payload = http.getString();
    JSONVar jsonResponse = JSON.parse(payload);

    if (JSON.typeof(jsonResponse) == "undefined") {
        Serial.println("Parsing input failed");
        return;
    }

    if (jsonResponse.hasOwnProperty("fault")) {
        Serial.print("playbackMetadata fault => ");
        Serial.println(jsonResponse["fault"]);
        http.end();
        return;
    }

#if DEBUG_RESPONSES
    Serial.print("playbackMetadata response => ");
    Serial.println(jsonResponse);
#endif

    _currentArtist = jsonResponse["currentItem"]["track"]["artist"]["name"];

    http.end();
}