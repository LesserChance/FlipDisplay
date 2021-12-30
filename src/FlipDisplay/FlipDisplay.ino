#include "FlipDisplay.h"
#include "FlipDisplayServer.h"

FlipDisplay display;
FlipDisplayServer server;

volatile bool buttonOneTriggered = false;
volatile bool buttonTwoTriggered = false;

void buttonOnePressed() {
    buttonOneTriggered = true;
    delay(100);
}

void buttonTwoPressed() {
    buttonTwoTriggered = true;
    delay(100);
}

void initializeGPIO() {
#if DEBUG || DEBUG_LOOP || DEBUG_LERP
    Serial.begin(115200);
    Serial.println("START");
#endif

    pinMode(enablePin, OUTPUT);

    for (int i = START_CHARACTER; i < CHARACTER_COUNT; i++) {
        pinMode(STEP_PIN[i], OUTPUT);
        pinMode(LOOP_PIN[i], INPUT);
    }

    pinMode(TEST_PIN_ONE, INPUT);
    pinMode(TEST_PIN_TWO, INPUT);

    attachInterrupt(TEST_PIN_ONE, buttonOnePressed, RISING); // yellow
    attachInterrupt(TEST_PIN_TWO, buttonTwoPressed, RISING); // green
}

void initializeServer() {
  server = FlipDisplayServer(display);
  server.setup();
}

void setup() {
    initializeGPIO();

    display = FlipDisplay();

#if SERVER_ENABLED
    initializeServer();
#endif
}

void loop() {
    if (buttonOneTriggered) {
        buttonOneTriggered = false;
        // do something
    }

    if (buttonTwoTriggered) {
        buttonTwoTriggered = false;
        // do something
    }

    display.run();
}
