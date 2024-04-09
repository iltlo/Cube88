#include <Arduino.h>
#include <time.h>

#include "matrix-pattern.hpp"
#include "hardware.hpp"
#include "menu.hpp"

byte timerMenu[7][8];

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;   // Define the mutex for the critical section
hw_timer_t * timer0;   // Declare hardware timer object
volatile int timerInterrupt = 0;

struct tm timeinfo;

int hour;
int minute;

void initClock() {
  if (isDebug) Serial.println("Initializing clock");

  configTime(28800, 0, "time.google.com", "pool.ntp.org", "ntp3.aliyun.com");  // GMT+8

  // get local time
  if (!getLocalTime(&timeinfo)) {
    if (isDebug) Serial.println("Failed to obtain time");
    return;
  }

  if (isDebug) Serial.print("Current time: " + String(asctime(&timeinfo)));
  if (isDebug) Serial.println("Clock initialized");
}

void getTime() {
  if (!getLocalTime(&timeinfo)) {
    if (isDebug) Serial.println("Failed to obtain time");
    return;
  }

  if (isDebug) Serial.print("Current time: " + String(asctime(&timeinfo)));

  hour = timeinfo.tm_hour;
  minute = timeinfo.tm_min;

  if (isDebug) Serial.println("Hour: " + String(hour) + " Minute: " + String(minute));
}

void ntpClock() {
  getTime();

  // TODO: Prompt for try again later if failed to obtain time

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
  delay(250);
}

void initTimerMenu() {
  for (int i = 0; i < sizeof(indexForTimer)/sizeof(indexForTimer[0]); i++){
    for (int j = 0; j < 8; j++){
      timerMenu[i][j] = numbers[indexForTimer[i]][j];
    }
  }

  // TODO: Add a user-defined option to set the timer
}

void IRAM_ATTR onTimer0() {
  portENTER_CRITICAL(&timerMux);
    timerInterrupt++;
  portEXIT_CRITICAL(&timerMux);
}

void countDownTimer(int timerMinutes) {
  if (isDebug) Serial.println("Countdown timer started");
  boolean isCancelled = false;

  timer0 = timerBegin(0, 80, true);   // Timer 0 with 80 prescaler (1MHz), auto-reload mode
  timerAttachInterrupt(timer0, &onTimer0, false);  // Attach the ISR to timer 0

  // Set the timer to count down from the given seconds
  timerAlarmWrite(timer0, 1000000, true);
  timerAlarmEnable(timer0);
  
  int minutesLeft = 0, secondsLeft = 0;
  int displayedMinute = 0, displayedSecond = 0;

  while (timerInterrupt < timerMinutes * 60) {
    // alarm has not been triggered

    // if (isDebug) Serial.println("Countdown: " + String(timerInterrupt));
    minutesLeft = (timerMinutes * 60 - timerInterrupt) / 60;
    secondsLeft = (timerMinutes * 60 - timerInterrupt) % 60;

    if (isDebug) Serial.println("Minutes left: " + String(minutesLeft) + " Seconds left: " + String(secondsLeft));
    
    if (minutesLeft == 0) {
      if (displayedSecond == secondsLeft) {
        continue;
      }
      scrollingAnimation(emptyLED, numbers[secondsLeft], 1);
      delay(350);
      scrollingAnimation(numbers[secondsLeft], emptyLED, 0);
    } else {
      // Flashes the remaining time every minute
      if (secondsLeft % 59 == 0 && secondsLeft != 0) {
        scrollingAnimation(emptyLED, numbers[minutesLeft], 1);
        delay(1000);
        scrollingAnimation(numbers[minutesLeft], emptyLED, 0);
        delay(350);
      } else {
        // Light up the corner LED to indicate the timer is running
        ledPrintByte(cornerLED);
      }
    }

    displayedMinute = minutesLeft;
    displayedSecond = secondsLeft;

    if (isShaking()) {
      if (isDebug) Serial.println("Timer cancelled");
      isCancelled = true;
      break;
    }

    delay(100);
  }

  // alarm has been triggered or timer has been cancelled
  if (isDebug) Serial.println("Countdown: " + String(timerInterrupt));

  timerAlarmDisable(timer0);
  timerEnd(timer0);

  timerInterrupt = 0;
  
  if (!isCancelled) {
    doubleBeep();

    if (isSilent) {
      // flash the corner LED to indicate the timer has ended
      for (int i = 0; i < 3; i++) {
        ledPrintByte(fullLED);
        delay(500);
        ledPrintByte(emptyLED);
        delay(500);
      }
    }
  }
  if (isDebug) Serial.println("Countdown timer ended");
  delay(250);
}

void showTimerMenu() {
  if (isDebug) Serial.println("Countdown timer");

  // Enable timer 0

  int selectedTimer = 0;
  int timerMinutes = 0;

  while (true) {
    selectedTimer = menuOptionsRender(timerMenu, selectedTimer, timerMenuSize-1);

    if (selectedTimer == -1) {
      if (isDebug) Serial.println("Exiting timer menu");
      doubleBeep();
      break;
    }

    if (isDebug) Serial.println("Selected " + String(selectedTimer));
    doubleBeep();

    timerMinutes = indexForTimer[selectedTimer];
    if (isDebug) Serial.println("Selected timer: " + String(timerMinutes) + " seconds");

    countDownTimer(timerMinutes);

    delay(250);
  }

}


