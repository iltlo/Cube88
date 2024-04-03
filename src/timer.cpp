#include <Arduino.h>
#include <time.h>

#include "matrix-pattern.hpp"
#include "hardware.hpp"
#include "menu.hpp"

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

void ntpClock() {
  getTime();

  byte hourPattern[8] = {0};
  byte minutePattern[8] = {0};
  
  for (int i = 0; i < 8; i++){
    // check if number is single digit
    hourPattern[i] = (hour < 10) ? twoDigit0to9[hour][i] : numbers[hour][i];
    minutePattern[i] = (minute < 10) ? twoDigit0to9[minute][i] : numbers[minute][i];
  }

  ledPrintByte(hourPattern);
  delay(750);
  scrollingAnimation(hourPattern, colon, 1);
  delay(250);
  scrollingAnimation(colon, minutePattern, 1);
}
