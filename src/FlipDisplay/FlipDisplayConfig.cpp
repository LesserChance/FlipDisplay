#include "FlipDisplayConfig.h"

void FlipDisplayConfig::initializePersistedValues() {
    Serial.println("Storing Initial Persisted Settings");

    initSPIFFS();

    setPersistedValue(ConfigKey::WIFI_SSID, DEFAULT_WIFI_SSID);
    setPersistedValue(ConfigKey::WIFI_PASSWORD, DEFAULT_WIFI_PASSWORD);
    setPersistedValue(ConfigKey::SONOS_ACCESS_TOKEN,
                      DEFAULT_SONOS_ACCESS_TOKEN);
    setPersistedValue(ConfigKey::SONOS_RESET_TOKEN,
                      DEFAULT_SONOS_RESET_TOKEN);
    setPersistedValue(ConfigKey::SONOS_GROUP_ID, DEFAULT_SONOS_GROUP_ID);
}

String FlipDisplayConfig::getPersistedValue(FlipDisplayConfig::ConfigKey key) {
    initSPIFFS();

    switch (key) {
        case ConfigKey::WIFI_SSID:
            return readFile(SPIFFS, WIFI_SSID_PATH);
            break;

        case ConfigKey::WIFI_PASSWORD:
            return readFile(SPIFFS, WIFI_PASSWORD_PATH);
            break;

        case ConfigKey::SONOS_ACCESS_TOKEN:
            return readFile(SPIFFS, SONOS_ACCESS_TOKEN_PATH);
            break;

        case ConfigKey::SONOS_RESET_TOKEN:
            return readFile(SPIFFS, SONOS_RESET_TOKEN_PATH);
            break;

        case ConfigKey::SONOS_GROUP_ID:
            return readFile(SPIFFS, SONOS_GROUP_ID_PATH);
            break;
    }

    // invalid key
    return String();
}

void FlipDisplayConfig::setPersistedValue(FlipDisplayConfig::ConfigKey key,
                                          const char *message) {
    initSPIFFS();

    switch (key) {
        case ConfigKey::WIFI_SSID:
            writeFile(SPIFFS, WIFI_SSID_PATH, message);
            break;

        case ConfigKey::WIFI_PASSWORD:
            writeFile(SPIFFS, WIFI_PASSWORD_PATH, message);
            break;

        case ConfigKey::SONOS_ACCESS_TOKEN:
            writeFile(SPIFFS, SONOS_ACCESS_TOKEN_PATH, message);
            break;

        case ConfigKey::SONOS_RESET_TOKEN:
            writeFile(SPIFFS, SONOS_RESET_TOKEN_PATH, message);
            break;

        case ConfigKey::SONOS_GROUP_ID:
            writeFile(SPIFFS, SONOS_GROUP_ID_PATH, message);
            break;
    }
}

void FlipDisplayConfig::deletePersistedValue(FlipDisplayConfig::ConfigKey key) {
    initSPIFFS();

    switch (key) {
        case ConfigKey::WIFI_SSID:
            deleteFile(SPIFFS, WIFI_SSID_PATH);
            break;

        case ConfigKey::WIFI_PASSWORD:
            deleteFile(SPIFFS, WIFI_PASSWORD_PATH);
            break;

        case ConfigKey::SONOS_ACCESS_TOKEN:
            deleteFile(SPIFFS, SONOS_ACCESS_TOKEN_PATH);
            break;

        case ConfigKey::SONOS_RESET_TOKEN:
            deleteFile(SPIFFS, SONOS_RESET_TOKEN_PATH);
            break;

        case ConfigKey::SONOS_GROUP_ID:
            deleteFile(SPIFFS, SONOS_GROUP_ID_PATH);
            break;
    }
}

void FlipDisplayConfig::initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
}

String FlipDisplayConfig::readFile(fs::FS &fs, const char *path) {
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

void FlipDisplayConfig::writeFile(fs::FS &fs, const char *path,
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

void FlipDisplayConfig::deleteFile(fs::FS &fs, const char *path) {
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