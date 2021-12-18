#include "Config.h"
#include "Secrets.h"
#include "FlipDisplay.h"
#include "FlipDisplayCharacter.h"
#include "FlipDisplayLerp.h"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define POLL_DURATION 1000000 * 15 // X seconds

volatile bool startLerp = false;
volatile bool startHome = false;
unsigned long lastPoll = 0;

FlipDisplay display;

void setup() {
    initializeGPIO();
    display = FlipDisplay();

    // pinMode(TEST_PIN_ONE, INPUT);
    // pinMode(TEST_PIN_TWO, INPUT);

    // attachInterrupt(TEST_PIN_ONE, triggerLerp, RISING); // yellow
    // attachInterrupt(TEST_PIN_TWO, triggerHome, RISING); // green

    // connectWifi();

    unsigned long _currentTime = micros();
    lastPoll = _currentTime;

    triggerHome();
}

void initializeGPIO() {
#if DEBUG
    Serial.begin(115200);
    Serial.println("START");
#endif

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        pinMode(STEP_PIN[i], OUTPUT);
        pinMode(LOOP_PIN[i], INPUT);
    }
}

void triggerLerp() {
    startLerp = true;
    delay(100);
}

void triggerHome() {
    startHome = true;
    delay(100);
}

void loop() {
    unsigned long _currentTime = micros();

    if (startHome) {
#if DEBUG
        Serial.println("HOMING");
#endif

        startHome = false;
        display.home();
    } else if (startLerp) {
#if DEBUG
        Serial.println("UPDATING");
#endif

        startLerp = false;
        display.setDisplay(words[random(0, WORD_COUNT)]);
        // requestWord();
    } else if (lastPoll + POLL_DURATION <= _currentTime) {
        startLerp = true;
        lastPoll = _currentTime;
    }

    display.run();
}

void connectWifi() {
    WiFi.begin(wifi_ssid, wifi_password);
  
    int connectCount = 0;
    while (connectCount < 10 && WiFi.status() != WL_CONNECTED) {
        delay(1000);
#if DEBUG
        Serial.println("Connecting to WiFi..");
#endif
        connectCount++;
    }
  
#if DEBUG
    if (connectCount == 10) {
        Serial.println("Failed to connect to the WiFi network");
    } else {
        Serial.println("Connected to the WiFi network");
    }
#endif
}

void requestWord() {
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
        HTTPClient http;
        http.begin("https://random-word-api.herokuapp.com/word?number=1");
        int httpCode = http.GET();
    
        if (httpCode > 0) { //Check for the returning code
            String payload = http.getString();
            display.setDisplay(payload.substring(2, payload.length() - 2));
        }
    
        http.end(); //Free the resources
    }
}