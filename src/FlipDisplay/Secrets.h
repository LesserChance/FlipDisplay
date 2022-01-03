#ifndef Secrets_h
#define Secrets_h

#include <Arduino.h>

extern const char* SERVER_IP;
extern const char* SERVER_URL;

extern const char* SONOS_API_KEY;
extern const char* SONOS_API_SECRET;
extern const char* SONOS_API_AUTH_B64;
extern const char* SONOS_HOUSEHOLD_ID;
extern const char* SONOS_REDIRECT_URL;

extern const char* DEFAULT_WIFI_SSID;
extern const char* DEFAULT_WIFI_PASSWORD;
extern const char* DEFAULT_SONOS_ACCESS_TOKEN;
extern const char* DEFAULT_SONOS_GROUP_ID;

extern const int WORD_COUNT;
extern String words[];

#endif