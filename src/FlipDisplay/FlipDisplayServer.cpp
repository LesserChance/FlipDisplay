
#include "FlipDisplayServer.h"

AsyncWebServer webserver(80);

String processor(const String &var) {
    if (var == "SERVER_URL") return SERVER_URL;
    if (var == "SONOS_API_KEY") return SONOS_API_KEY;
    if (var == "SONOS_API_AUTH_B64") return SONOS_API_AUTH_B64;
    if (var == "SONOS_REDIRECT_URL") return SONOS_REDIRECT_URL;

    return String();
}

FlipDisplayServer::FlipDisplayServer() {}

FlipDisplayServer::FlipDisplayServer(FlipDisplay *display) {
    _display = display;
}

void FlipDisplayServer::setupServer() {
    initSPIFFS();

    // Load values saved in SPIFFS
    _ssid = FlipDisplayConfig::getPersistedValue(
        FlipDisplayConfig::ConfigKey::WIFI_SSID);
    _pass = FlipDisplayConfig::getPersistedValue(
        FlipDisplayConfig::ConfigKey::WIFI_PASSWORD);

    if (_ssid == "") {
        // there is not yet an SSID defined, which means the data directory has
        // been wiped
        FlipDisplayConfig::initializePersistedValues();

        _ssid = FlipDisplayConfig::getPersistedValue(
            FlipDisplayConfig::ConfigKey::WIFI_SSID);
        _pass = FlipDisplayConfig::getPersistedValue(
            FlipDisplayConfig::ConfigKey::WIFI_PASSWORD);
    }

    // try to connect to the wifi with the given credentials
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
        request->send(SPIFFS, "/wifi-setup.html", String(), false, processor);
    });

    setupRouting();

    webserver.begin();
}

void FlipDisplayServer::initServer() {
    // serve the apropriate index
    webserver.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", String(), false, processor);
    });

    setupRouting();

    webserver.begin();
}

void FlipDisplayServer::setupRouting() {
    webserver.serveStatic("/", SPIFFS, "/");

    // non-static pages
    webserver.on("/config", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/config.html", String(), false, processor);
    });

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

    // Sonos APIs
    webserver.on(
        "/updateSonosAccess", HTTP_GET,
        [this](AsyncWebServerRequest *request) { updateSonosAccess(request); });
    webserver.on("/sonos", HTTP_POST, [this](AsyncWebServerRequest *request) {
        updateSonosSettings(request);
    });
    webserver.on("/sonosGroups", HTTP_GET,
                 [this](AsyncWebServerRequest *request) {
                     displaySonosGroups(request);
                 });
}

void FlipDisplayServer::setWifi(AsyncWebServerRequest *request) {
    if (!request->hasArg("ssid") || !request->hasArg("pass")) {
        request->send(400, "text/plain", "REQUIRES ARGUMENTS: ssid/pass");
        return;
    }

    _ssid = request->arg("ssid");
    _pass = request->arg("pass");

    FlipDisplayConfig::setPersistedValue(
        FlipDisplayConfig::ConfigKey::WIFI_SSID, _ssid.c_str());
    FlipDisplayConfig::setPersistedValue(
        FlipDisplayConfig::ConfigKey::WIFI_PASSWORD, _pass.c_str());

    Serial.print("SSID set to: ");
    Serial.println(_ssid);

    request->send(SPIFFS, "/success-restart.html", String(), false, processor);

    _display->triggerRestart(3);
}

void FlipDisplayServer::enableDisplayMotors(AsyncWebServerRequest *request) {
    _display->enable(true);
    request->send(200, "text/html", "ACK");
}

void FlipDisplayServer::disableDisplayMotors(AsyncWebServerRequest *request) {
    _display->disable(true);
    request->send(200, "text/html", "ACK");
}

void FlipDisplayServer::homeDisplay(AsyncWebServerRequest *request) {
    _display->home();
    request->send(200, "text/html", "ACK");
}

void FlipDisplayServer::stepCharacter(AsyncWebServerRequest *request) {
    if (request->hasArg("character") == false) {
        Serial.println("FAIL");
        request->send(400, "text/html", "REQUIRES ARGUMENT 'character'");
        return;
    }

    int characterIndex = request->arg("character").toInt();
    int offsetPosition = _display->stepCharacter(characterIndex);

    Serial.print("characterIndex: ");
    Serial.println(characterIndex);

    Serial.print("offsetPosition: ");
    Serial.println(offsetPosition);

    request->send(200, "text/html", String(offsetPosition));
}

void FlipDisplayServer::setDisplay(AsyncWebServerRequest *request) {
    if (request->hasArg("text") == false) {
        Serial.println("FAIL");
        request->send(400, "text/html", "REQUIRES ARGUMENT 'text'");
        return;
    }

    String body = request->arg("text");
    _display->setDisplay(body);

    // Respond to the client
    request->send(200, "text/html", body);
}

void FlipDisplayServer::randomWord(AsyncWebServerRequest *request) {
    String word = words[random(0, WORD_COUNT)];
    _display->setDisplay(word);

    // Respond to the client
    request->send(200, "text/html", word);
}

void FlipDisplayServer::updateSonosAccess(AsyncWebServerRequest *request) {
    String code = request->arg("code");
    Serial.print("code: ");
    Serial.println(code);

    String curl =
        "curl -X POST -H \"Content-Type:"
        " application/x-www-form-urlencoded;charset=utf-8\""
        " -H \"Authorization: Basic {" +
        String(SONOS_API_AUTH_B64) +
        "}\""
        " \"https://api.sonos.com/login/v3/oauth/access\""
        " -d \"grant_type=authorization_code&code=" +
        code +
        "&redirect_uri=https://ryan-bateman.com/\"";

        request->send(200, "text/html", curl);
}

void FlipDisplayServer::updateSonosSettings(AsyncWebServerRequest *request) {
    if (request->hasArg("access_token")) {
        String access_token = request->arg("access_token");
        if (access_token != "") {
            FlipDisplayConfig::setPersistedValue(
                FlipDisplayConfig::ConfigKey::SONOS_ACCESS_TOKEN,
                access_token.c_str());

            Serial.print("Stored access_token: ");
            Serial.println(access_token);
        }
    }

    if (request->hasArg("reset_token")) {
        String reset_token = request->arg("reset_token");
        if (reset_token != "") {
            FlipDisplayConfig::setPersistedValue(
                FlipDisplayConfig::ConfigKey::SONOS_RESET_TOKEN,
                reset_token.c_str());

            Serial.print("Stored reset: ");
            Serial.println(reset_token);
        }
    }

    if (request->hasArg("group_id")) {
        String group_id = request->arg("group_id");
        FlipDisplayConfig::setPersistedValue(
            FlipDisplayConfig::ConfigKey::SONOS_GROUP_ID, group_id.c_str());

        Serial.print("Stored group_id: ");
        Serial.println(group_id);
    }

    request->send(SPIFFS, "/success-restart.html", String(), false, processor);

    _display->triggerRestart(3);
}

void FlipDisplayServer::displaySonosGroups(AsyncWebServerRequest *request) {
    if (WiFi.status() == WL_CONNECTED) {
        String url = "https://api.ws.sonos.com/control/api/v1/households/" +
                     String(SONOS_HOUSEHOLD_ID) + "/groups";

        HTTPClient http;
        http.begin(url.c_str());

        http.addHeader(
            "Authorization",
            "Bearer " + String(FlipDisplayConfig::getPersistedValue(
                            FlipDisplayConfig::ConfigKey::SONOS_ACCESS_TOKEN)));
        http.addHeader("Content-Type", "application/json");

        int httpCode = http.sendRequest("GET");

        if (httpCode == 0) {
#if DEBUG_RESPONSES
            Serial.println("http failed");
#endif
            return;
        }

        String payload = http.getString();
        request->send(200, "text/html", payload);

        http.end();
    }
}