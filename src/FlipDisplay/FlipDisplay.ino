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

void initializeGPIO() {
#if DEBUG || DEBUG_LOOP || DEBUG_LERP || DEBUG_RESPONSES
    Serial.begin(115200);
    Serial.println("START");
#endif

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
    display.setupDisplay(false);

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

    // todo: switch between active programs
    // progClock.run(buttonOneTriggered, buttonTwoTriggered);
    progSonos.run(buttonOneTriggered, buttonTwoTriggered);

    display.run();
}
