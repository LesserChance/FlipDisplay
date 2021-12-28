// FlipDisplayServer.cpp

#include "FlipDisplayServer.h"
#include "Secrets.h"

FlipDisplay display;
WebServer server(80);

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

void setupDisplay() {
    initializeGPIO();
    display = FlipDisplay();

#if SERVER_ENABLED
    initializeServer();
#endif
}

void loopDisplay() {
    unsigned long _currentTime = micros();

    if (buttonOneTriggered) {
        buttonOneTriggered = false;
        // do something
    }

    if (buttonTwoTriggered) {
        buttonTwoTriggered = false;
        // do something
    }

#if SERVER_ENABLED
    server.handleClient();
#endif

    display.run();
}

void initializeServer() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    connectWifi();
    setupRouting();
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

    // pinMode(TEST_PIN_ONE, INPUT);
    // pinMode(TEST_PIN_TWO, INPUT);

    // attachInterrupt(TEST_PIN_ONE, buttonOnePressed, RISING); // yellow
    // attachInterrupt(TEST_PIN_TWO, buttonTwoPressed, RISING); // green
}

void connectWifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int connectCount = 0;
    while (connectCount < 20 && WiFi.status() != WL_CONNECTED) {
        delay(1000);
#if DEBUG
        Serial.println("Connecting to WiFi..");
#endif
        connectCount++;
    }

#if DEBUG
    if (connectCount == 20) {
        Serial.println("Failed to connect to the WiFi network");
    } else {
        Serial.println("Connected to the WiFi network");
        Serial.print("Server IP: ");
        Serial.println(WiFi.localIP());
    }
#endif
}

void setupRouting() {
    // static routes
    server.serveStatic("/", SPIFFS, "/index.html");
    server.serveStatic("/config", SPIFFS, "/config.html");
    
    // API
    server.on("/home", homeDisplay);
    server.on("/update", HTTP_POST, setDisplay);
    server.on("/step", HTTP_POST, stepCharacter);
    server.on("/word", randomWord);
    
    server.on("/enable", HTTP_POST, enableDisplayMotors);
    server.on("/disable", HTTP_POST, disableDisplayMotors);

    // start server
    server.begin();
}

void enableDisplayMotors() {
    display.enable(true);
    server.send(200, "text/html", "ACK");
}

void disableDisplayMotors() {
    display.disable(true);
    server.send(200, "text/html", "ACK");
}

void homeDisplay() {
    display.home();
    server.send(200, "text/html", "ACK");
}

void stepCharacter() {
    Serial.println("stepCharacter");
    if (server.hasArg("character") == false) {
        // handle error here
        Serial.println("FAIL");
        server.send(400, "text/html", "REQUIRES ARGUMENT 'character'");
        return;
    }

    int characterIndex = server.arg("character").toInt();
    int offsetPosition = display.stepCharacter(characterIndex);

    Serial.print("characterIndex: ");
    Serial.println(characterIndex);

    Serial.print("offsetPosition: ");
    Serial.println(offsetPosition);

    server.send(200, "text/html", String(offsetPosition));
}

void setDisplay() {
    Serial.println("handlePost");
    if (server.hasArg("text") == false) {
        // handle error here
        Serial.println("FAIL");
        server.send(400, "text/html", "REQUIRES ARGUMENT 'text'");
        return;
    }

    String body = server.arg("text");
    display.setDisplay(body);

    // Respond to the client
    server.send(200, "text/html", body);
}

void randomWord() {
    Serial.println("randomWord");

    String word = words[random(0, WORD_COUNT)];
    display.setDisplay(word);

    // Respond to the client
    server.send(200, "text/html", word);
}