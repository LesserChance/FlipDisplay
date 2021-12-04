#include "FlipDisplay.h"
#include "FlipDisplayCharacter.h"
#include "FlipDisplayLerp.h"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define MOTOR_SR_CLOCK_PIN 21   // pin 11 on the 74hc595, blue
#define MOTOR_SR_LATCH_PIN 22   // pin 12 on the 74hc595, green RCLK
#define MOTOR_SR_DATA_PIN  23   // pin 14 on the 74hc595, yellow

#define TEST_PIN_ONE 5 // yellow
#define TEST_PIN_TWO 4 // green

#define POLL_DURATION 1000000 * 60 // 60 second

#define DEBUG 1

byte lastStepPinRegisterOutput = 0b00000000;

volatile bool startLerp = false;
volatile bool startHome = false;
unsigned long lastPoll = 0;

const char* ssid = "****";
const char* password = "****";

const int WORD_COUNT = 1;
String words[WORD_COUNT] = {"test"};

FlipDisplay display;

void setup() {
#if DEBUG
    Serial.begin(115200);
    Serial.println("START");
#endif
    display = FlipDisplay();

    pinMode(MOTOR_SR_LATCH_PIN, OUTPUT);
    pinMode(MOTOR_SR_CLOCK_PIN, OUTPUT);
    pinMode(MOTOR_SR_DATA_PIN, OUTPUT);
    pinMode(TEST_PIN_ONE, INPUT);
    pinMode(TEST_PIN_TWO, INPUT);

    attachInterrupt(TEST_PIN_ONE, triggerLerp, RISING); // yellow
    attachInterrupt(TEST_PIN_TWO, triggerHome, RISING); // green

    connectWifi();

    startHome = true;
}

void triggerLerp() {
    startLerp = true;
    delay(100);
}

void triggerHome() {
    startHome = true;
    delay(100);
}

void testShiftRegister() {
    digitalWrite(MOTOR_SR_LATCH_PIN, LOW);
    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, MSBFIRST, 0b00000000);   // STEP PINS
    digitalWrite(MOTOR_SR_LATCH_PIN, HIGH);
    delay(1000);

    digitalWrite(MOTOR_SR_LATCH_PIN, LOW);
    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, MSBFIRST, 0b11111111);   // STEP PINS
    digitalWrite(MOTOR_SR_LATCH_PIN, HIGH);
    delay(1000);
}

void testButtons() {
    Serial.print("1: ");
    Serial.println(digitalRead(CHAR_ONE_BUTTON_PIN));
    Serial.print("2: ");
    Serial.println(digitalRead(CHAR_TWO_BUTTON_PIN));
    Serial.print("3: ");
    Serial.println(digitalRead(CHAR_THREE_BUTTON_PIN));
    Serial.print("4: ");
    Serial.println(digitalRead(CHAR_FOUR_BUTTON_PIN));
    Serial.print("5: ");
    Serial.println(digitalRead(CHAR_FIVE_BUTTON_PIN));
    Serial.print("6: ");
    Serial.println(digitalRead(CHAR_SIX_BUTTON_PIN));
    delay(1000);
}

void loop() {
    unsigned long _currentTime = micros();
    if (startHome) {
        startHome = false;
#if DEBUG
        Serial.println("HOMING");
#endif
        display.home();
    }
    else if (startLerp) {
        startLerp = false;
#if DEBUG
        Serial.println("UPDATING");
#endif
        display.setDisplay(words[random(0, WORD_COUNT)]);
        // requestWord();
    }
    else if (lastPoll + POLL_DURATION <= _currentTime) {
      lastPoll = _currentTime;
      startLerp = true;
    }

    display.run();
    updateRegisters();
}

void updateRegisters() {
    byte stepPinRegisterOutput = display.getRegisterOutput();
    if (lastStepPinRegisterOutput != stepPinRegisterOutput) {
        lastStepPinRegisterOutput = stepPinRegisterOutput;

        digitalWrite(MOTOR_SR_LATCH_PIN, LOW);
        shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, MSBFIRST, stepPinRegisterOutput);
        digitalWrite(MOTOR_SR_LATCH_PIN, HIGH);
    }
}

void printBinary(byte b) {
#if DEBUG
    for (int i = 7; i >= 0; i--) {
        Serial.print((b >> i) & 0X01);
    }
    Serial.println();
#endif
}

void connectWifi() {
    WiFi.begin(ssid, password);
  
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