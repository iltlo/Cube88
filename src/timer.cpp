#include <Arduino.h>
#include <time.h>

struct tm timeinfo;

int hour;
int minute;

void initClock() {
  Serial.println("Initializing clock");

  configTime(28800, 0, "stdtime.gov.hk", "pool.ntp.org", "ntp3.aliyun.com");  // GMT+8

  // Serial.println("Waiting for NTP time sync: ");
  // time_t now = time(nullptr);
  // while (now < 8 * 3600 * 2) {
  //   delay(500);
  //   Serial.print(".");
  //   now = time(nullptr);
  // }
  // Serial.println("");
  // gmtime_r(&now, &timeinfo);

  // get local time
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.print("Current time: " + String(asctime(&timeinfo)));
  Serial.println("Clock initialized");
}

void getTime() {
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.print("Current time: " + String(asctime(&timeinfo)));

  hour = timeinfo.tm_hour;
  minute = timeinfo.tm_min;

  Serial.println("Hour: " + String(hour) + " Minute: " + String(minute));
}
