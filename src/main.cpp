#include <Arduino.h>
#include <Wifi.h>

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

  connectWiFi();

  initClock();
  
  WiFi.disconnect(true);  // disconnect from wifi after obtaining time
  WiFi.mode(WIFI_OFF);    // turn off wifi
  if (isDebug) Serial.println("Wifi status: " + String(WiFi.status()));

  initTimerMenu();
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
      if (isDebug) Serial.println("Silent mode: " + isSilent ? "ON" : "OFF");
      break;
    case 5:
      // Reconnect to WiFi and update time
      break;
    case 6:
      // function 6
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
