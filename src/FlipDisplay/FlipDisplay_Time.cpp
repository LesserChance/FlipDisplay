#include "FlipDisplay_Time.h"

char currentTimeString[4];

void setTimeByNTP() {
  configTime(GMT_OFFSET * 3600, 0, "pool.ntp.org");
}

String getTime() {
  //using delays with the motors fucks with the time - we need to get rid of those 
  setTimeByNTP();

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  
  sprintf(currentTimeString, "%02d%02d", timeinfo.tm_hour % 12, timeinfo.tm_min);

  return String(currentTimeString);
}