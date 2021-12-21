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
    connectWifi();
    setupRouting();
}

void initializeGPIO() {
#if DEBUG
    Serial.begin(115200);
    Serial.println("START");
#endif

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
    server.on("/", displayForm);
    server.on("/home", homeDisplay);
    server.on("/update", HTTP_POST, setDisplay);
    server.on("/word", randomWord);

    // start server
    server.begin();
}

void displayForm() {
    server.send(200, "text/html", 
        "<!DOCTYPE html><title>Flip Display</title><meta content='width=device-width,initial-scale=1'name=viewport><link href=https://stackpath.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css rel=stylesheet><link href=https://stackpath.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap-theme.min.css rel=stylesheet><style>html{height:100%}body{background-color:#49b96f;background:linear-gradient(110deg,rgba(73,185,111,1) 74%,rgba(25,116,56,1) 100%);min-height:40rem;height:100%;padding:2rem}.container{height:100%;padding:0}.form-container{background-color:#2a2a2c;color:#FFF;padding:3rem;box-shadow:5px 5px 15px 7px #343;border-radius:5px;border:1px solid #111}img{max-width:100%}button,input,label{margin-right:.5rem}.f{display:inline-block;margin:1rem 0}.container{max-width:970px}</style><div class=container><div class='container form-container'><img class=center-block src=https://i.postimg.cc/3xtR7F4Q/Flip.png><br><br><form action=/update class=form-inline target=ft method=POST><div class=form-group><label>Update Display:</label><input class=form-control name=text></div><button class='btn btn-primary'type=submit>Update</button></form><br><br><label>Other Actions:</label><br><form action=/word class=f target=ft><button class='btn btn-primary'type=submit>Random Word</button></form><form action=/home class=f target=ft><button class='btn btn-success'type=submit>Re-home</button></form><iframe id=ft name=ft style=display:none></iframe></div></div>"
    );
}

void homeDisplay() {
    display.home();
    server.send(200, "text/html", "ACK");
}

void setDisplay() {
    Serial.println("handlePost");
    if (server.hasArg("text") == false) {
        //handle error here
        Serial.println("FAIL");
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