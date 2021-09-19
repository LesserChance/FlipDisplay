#include "FlipDisplay_Wifi.h"

const char* ssid = "XXXX";
const char* password = "XXXX";

void connectWifi() {
  WiFi.begin(ssid, password);
 
  int connectCount = 0;
  while (connectCount < 10 && WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    connectCount++;
  }
 
  if (connectCount == 10) {
    Serial.println("Failed to connect to the WiFi network");
  } else {
    Serial.println("Connected to the WiFi network");
  }
}

void requestWord() {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
    http.begin("https://random-word-api.herokuapp.com/word?number=1");
    int httpCode = http.GET();
 
    if (httpCode > 0) { //Check for the returning code
      String payload = http.getString();
      setDisplayString(payload.substring(2, payload.length() - 2));
    } else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
}
