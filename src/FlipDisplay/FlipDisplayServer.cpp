
#include "FlipDisplayServer.h"
#include "Secrets.h"

AsyncWebServer webserver(80);

FlipDisplayServer::FlipDisplayServer() {}

FlipDisplayServer::FlipDisplayServer(FlipDisplay display) {
    _display = display;
}

void FlipDisplayServer::setup() {
    initSPIFFS();

    // Load values saved in SPIFFS
    _ssid = readFile(SPIFFS, _ssidPath);
    _pass = readFile(SPIFFS, _passPath);

    // try to connect ot the wifi with the given credentials
    if (initWiFi()) {
        initServer();
    } else {
        initSoftAP();
    }
}

void FlipDisplayServer::initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
}

String FlipDisplayServer::readFile(fs::FS &fs, const char *path) {
    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        Serial.println("- failed to open file for reading");
        return String();
    }

    String fileContent;
    while (file.available()) {
        fileContent = file.readStringUntil('\n');
        break;
    }
    return fileContent;
}

void FlipDisplayServer::writeFile(fs::FS &fs, const char *path,
                                  const char *message) {
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (!file.print(message)) {
        Serial.println("- frite failed");
    }
}

void FlipDisplayServer::deleteFile(fs::FS &fs, const char *path) {
    Serial.printf("Deleting file: %s\r\n", path);

    if (!fs.exists(path)) {
        Serial.println("- failed to locate file for deleting");
        return;
    }

    boolean success = fs.remove(path);
    if (!success) {
        Serial.println("- delete failed");
    }
}

bool FlipDisplayServer::initWiFi() {
    if (_ssid == "") {
        Serial.println("Undefined SSID");
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid.c_str(), _pass.c_str());

    Serial.println("Connecting to WiFi...");

    int connectCount = 0;
    while (connectCount < WIFI_FAIL_COUNT && WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
        connectCount++;
    }

    if (connectCount >= WIFI_FAIL_COUNT) {
        Serial.println("Failed to connect to the WiFi network");
        return false;
    } else {
        Serial.println("Connected to the WiFi network");
        Serial.print("Server IP: ");
        Serial.println(WiFi.localIP());
    }

    return true;
}

void FlipDisplayServer::initSoftAP() {
    WiFi.softAP("FLIP-DISPLAY", NULL);
    IPAddress IP = WiFi.softAPIP();

    Serial.print("Access Point IP: ");
    Serial.println(IP);

    // serve the apropriate index
    webserver.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/wifi-setup.html", "text/html");
    });

    setupRouting();

    webserver.begin();
}

void FlipDisplayServer::initServer() {
    // serve the apropriate index
    webserver.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    setupRouting();

    webserver.begin();
}

void FlipDisplayServer::resetSSID() {
    deleteFile(SPIFFS, _ssidPath);
    deleteFile(SPIFFS, _passPath);
}

void FlipDisplayServer::setupRouting() {
    webserver.serveStatic("/", SPIFFS, "/");

    // config API
    webserver.on("/wifi", HTTP_POST,
                 [this](AsyncWebServerRequest *request) { setWifi(request); });
    webserver.on("/enable", HTTP_POST, [this](AsyncWebServerRequest *request) {
        enableDisplayMotors(request);
    });
    webserver.on("/disable", HTTP_POST, [this](AsyncWebServerRequest *request) {
        disableDisplayMotors(request);
    });

    // Flip Display API
    webserver.on("/home", HTTP_POST, [this](AsyncWebServerRequest *request) {
        homeDisplay(request);
    });
    webserver.on("/update", HTTP_POST, [this](AsyncWebServerRequest *request) {
        setDisplay(request);
    });
    webserver.on("/step", HTTP_POST, [this](AsyncWebServerRequest *request) {
        stepCharacter(request);
    });
    webserver.on("/word", HTTP_POST, [this](AsyncWebServerRequest *request) {
        randomWord(request);
    });
}

void FlipDisplayServer::setWifi(AsyncWebServerRequest *request) {
    if (!request->hasArg("ssid") || !request->hasArg("pass")) {
        request->send(400, "text/plain", "REQUIRES ARGUMENTS: ssid/pass");
        return;
    }

    _ssid = request->arg("ssid").c_str();
    _pass = request->arg("pass").c_str();

    writeFile(SPIFFS, _ssidPath, _ssid.c_str());
    writeFile(SPIFFS, _passPath, _pass.c_str());

    Serial.print("SSID set to: ");
    Serial.println(_ssid);

    request->send(SPIFFS, "/wifi-success.html", "text/html");

    delay(3000);

    ESP.restart();
}

void FlipDisplayServer::enableDisplayMotors(AsyncWebServerRequest *request) {
    _display.enable(true);
    request->send(200, "text/html", "ACK");
}

void FlipDisplayServer::disableDisplayMotors(AsyncWebServerRequest *request) {
    _display.disable(true);
    request->send(200, "text/html", "ACK");
}

void FlipDisplayServer::homeDisplay(AsyncWebServerRequest *request) {
    _display.home();
    request->send(200, "text/html", "ACK");
}

void FlipDisplayServer::stepCharacter(AsyncWebServerRequest *request) {
    if (request->hasArg("character") == false) {
        // handle error here
        Serial.println("FAIL");
        request->send(400, "text/html", "REQUIRES ARGUMENT 'character'");
        return;
    }

    int characterIndex = request->arg("character").toInt();
    int offsetPosition = _display.stepCharacter(characterIndex);

    Serial.print("characterIndex: ");
    Serial.println(characterIndex);

    Serial.print("offsetPosition: ");
    Serial.println(offsetPosition);

    request->send(200, "text/html", String(offsetPosition));
}

void FlipDisplayServer::setDisplay(AsyncWebServerRequest *request) {
    if (request->hasArg("text") == false) {
        // handle error here
        Serial.println("FAIL");
        request->send(400, "text/html", "REQUIRES ARGUMENT 'text'");
        return;
    }

    String body = request->arg("text");
    _display.setDisplay(body);

    // Respond to the client
    request->send(200, "text/html", body);
}

void FlipDisplayServer::randomWord(AsyncWebServerRequest *request) {
    String word = words[random(0, WORD_COUNT)];
    _display.setDisplay(word);

    // Respond to the client
    request->send(200, "text/html", word);
}