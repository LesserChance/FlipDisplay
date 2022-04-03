#include "FlipDisplay.h"
#include "FlipDisplayServer.h"
#include "ProgramClock.h"
#include "ProgramSonos.h"

FlipDisplay display;
FlipDisplayServer server;
ProgramClock progClock;
ProgramSonos progSonos;

bool buttonOneTriggered = false;
bool buttonTwoTriggered = false;
uint8_t buttonOneLastState;
uint8_t buttonTwoLastState;
unsigned long buttonOneDebounceStart = 0;
unsigned long buttonTwoDebounceStart = 0;

uint8_t currentProgram = 0;

void initializeGPIO() {
    Serial.begin(115200);
    Serial.println("START");

    pinMode(enablePin, OUTPUT);

    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        pinMode(STEP_PIN[i], OUTPUT);
        pinMode(LOOP_PIN[i], INPUT);
        digitalWrite(STEP_PIN[i], LOW);
    }

    pinMode(TEST_PIN_ONE, INPUT_PULLUP);
    pinMode(TEST_PIN_TWO, INPUT_PULLUP);
    
    buttonOneLastState = digitalRead(TEST_PIN_ONE);
    buttonTwoLastState = digitalRead(TEST_PIN_TWO);
}

void initializePrograms() {
    progClock = ProgramClock(&display);
    progClock.setupProgram();

    progSonos = ProgramSonos(&display);
    progSonos.setupProgram();
}

void setup() {
    initializeGPIO();

    display = FlipDisplay();
    display.setupDisplay();

    server = FlipDisplayServer(&display);
    server.setupServer();
    
    initializePrograms();
}

unsigned long lastTime = 0;

void readButtons() {
    if (buttonOneTriggered) {
        Serial.println("BUTTON ONE");
        buttonOneTriggered = false;
    }

    if (buttonTwoTriggered) {
        Serial.println("BUTTON TWO");
        buttonTwoTriggered = false;
    }

    uint8_t buttonOneState = digitalRead(TEST_PIN_ONE);
    uint8_t buttonTwoState = digitalRead(TEST_PIN_TWO);
    unsigned long currentTime = millis();

    if (buttonOneDebounceStart == 0 && buttonOneState != buttonOneLastState) {
        buttonOneDebounceStart = currentTime;
    }
    if (buttonOneDebounceStart != 0 && buttonOneDebounceStart + 100 < currentTime) {
        buttonOneTriggered = true;
        buttonOneDebounceStart = 0;
        buttonOneLastState = buttonOneState;
    }

    if (buttonTwoDebounceStart == 0 && buttonTwoState != buttonTwoLastState) {
        buttonTwoDebounceStart = currentTime;
    }
    if (buttonTwoDebounceStart != 0 && buttonTwoDebounceStart + 100 < currentTime) {
        buttonTwoTriggered = true;
        buttonTwoDebounceStart = 0;
        buttonTwoLastState = buttonTwoState;
    }
}

void loop() {
    readButtons();

    if (buttonTwoTriggered) {
        currentProgram = (currentProgram + 1) % 3;
        switch (currentProgram) {
            case 0:
                display._programName = "Clock";
                Serial.println("PROGRAM: CLOCK");
                break;
            case 1:
                display._programName = "Sonos";
                Serial.println("PROGRAM: SONOS");
                break;
            case 2:
                display._programName = "None";
                Serial.println("PROGRAM: NONE");
                break;
        }
    }

    switch (currentProgram) {
        case 0:
            progClock.run(buttonOneTriggered, buttonTwoTriggered);
            break;
        case 1:
            progSonos.run(buttonOneTriggered, buttonTwoTriggered);
            break;
        case 2:
            // no program
            if (buttonOneTriggered) {
                String word = words[random(0, WORD_COUNT)];
                display.setDisplay(word, MICROSECONDS * 30, true);
            }
            break;
    }

    display.run();
}