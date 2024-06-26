#include <Arduino.h>
#include <Wifi.h>
#include <esp_sleep.h>

#include "hardware.hpp"
#include "wifi-info.hpp"
#include "timer.hpp"
#include "menu.hpp"
#include "snake.hpp"

#include "matrix-pattern.hpp"

boolean isSilent = false;

int prevPage = 0, selectedPage = 0;

// Function prototypes
void chooseMenuOption(int selectedPage);
void rollDice();
void connectWiFi();

// Main setup and loop
void setup() {
  Serial.begin(115200);
  initHardware();

  if (getBatteryLevel() < 3.0) {
    // battery is lower than 3.0V, go to deep sleep mode
    if (isDebug) Serial.println("Battery level is low, going to deep sleep");
    // Flash the battery icon for 3 seconds
    for (int i = 0; i < 3; i++) {
      beep();
      ledPrintByte(mainMenu[5]);  // battery icon
      delay(250);
      ledPrintByte(emptyLED);
      delay(250);
    }
    esp_deep_sleep_start();
  }

  connectWiFi();

  initClock();
  
  WiFi.disconnect(true);  // disconnect from wifi after obtaining time
  WiFi.mode(WIFI_OFF);    // turn off wifi
  if (isDebug) Serial.println("Wifi status: " + String(WiFi.status()));

  initTimerMenu();

  if (isDebug) Serial.println("CPU Frequency: " + String(F_CPU));
}

void loop() {
  prevPage = selectedPage;
  selectedPage = menuOptionsRender(mainMenu, selectedPage, mainMenuSize-1);

  if (selectedPage != -1) {
    if (isDebug) Serial.println("Selected " + String(selectedPage));
    doubleBeep();

    // Enter the selected menu's function
    chooseMenuOption(selectedPage);
  } else {
    selectedPage = prevPage;
  }
  delay(250);

}

// Function definitions
void chooseMenuOption(int selectedPage) {
  switch(selectedPage) {
    case 0:
      // 24-hour clock
      ntpClock();
      break;
    case 1:
      // Countdown timer
      showTimerMenu();
      break;
    case 2:
      // 6 sided dice
      rollDice();
      break;
    case 3:
      // Snake game
      snakeGame();
      break;
    case 4:
      // Toggle silent mode
      isSilent = !isSilent;
      if (!isSilent) doubleBeep();
      if (isDebug) Serial.println("Silent mode: " + String(isSilent));
      break;
    case 5:
      // Display battery level
      showBatteryLevel();
      break;
    case 6:
      // Use serial to print the orientation
      while (1) {
        printOrientation();
        if (isShaking()) {
          doubleBeep();
          delay(600);
          break;
        }
      }
      break;
    default:
      break;
  }
}

void rollDice() {
  int diceRoll = random(1, 7);
  
  scrollingAnimation(emptyLED, d6Dice[diceRoll - 1], 1);
  delay(500);
  ledPrintByte(emptyLED);
  delay(250);

  if (isDebug) Serial.println("Dice rolled: " + String(diceRoll));
  for (int i = 0; i < 3; i++) {
    ledPrintByte(d6Dice[diceRoll - 1]);
    beep();
    delay(250);
    if (i < 2) {
      ledPrintByte(emptyLED);
      delay(200);
    } else {
      delay(500);
      scrollingAnimation(d6Dice[diceRoll - 1], emptyLED, 0);
    }
  }
  delay(300);
}

void connectWiFi() {
  WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);

  WiFi.begin(ssid, password);
  // connect to wifi, only wait for 5 seconds
  if (isDebug) Serial.println("Connecting to WiFi");
  for (int i = 0; i < 5; i++) {
    if (isDebug) Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) {
      if (isDebug) Serial.println("\nConnected to the WiFi network");
      if (isDebug) Serial.println("IP address: " + WiFi.localIP().toString());
      break;
    }
    delay(1000);
  }
}
