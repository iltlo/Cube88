#include <Arduino.h>
#include <Wifi.h>

#include "hardware.hpp"
#include "wifi-info.hpp"
#include "timer.hpp"
#include "menu.hpp"

#include "matrix-pattern.hpp"

boolean isSilent = false;

int prevPage = 0, selectedPage = 0;

void chooseMenuOption(int selectedPage) {
  switch(selectedPage) {
    case 0:
      // function 0
      ntpClock();
      break;
    case 1:
      // function 1
      showTimerMenu();
      break;
    case 2:
      // function 2
      // rollDice();
      break;
    case 3:
      // function 3
      isSilent = !isSilent;
      if (isDebug) Serial.println("Silent mode: " + isSilent ? "ON" : "OFF");
      break;
    case 4:
      // function 4
      break;
    case 5:
      // function 5
      break;
    case 6:
      // function 6
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  initHardware();

  WiFi.begin(ssid, password);
  // connect to wifi, only wait for 5 seconds
  if (isDebug) Serial.print("Connecting to WiFi");
  for (int i = 0; i < 5; i++) {
    if (isDebug) Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) {
      if (isDebug) Serial.print("\nConnected to the WiFi network");
      break;
    }
    delay(1000);
  }
  if (isDebug) Serial.println();

  initClock();
  
  WiFi.disconnect(true);  // disconnect from wifi after obtaining time
  WiFi.mode(WIFI_OFF);    // turn off wifi

  initTimerMenu();
}

void loop() {
  prevPage = selectedPage;
  selectedPage = menuOptionsRender(mainMenu, selectedPage, mainMenuSize-1);  // 0-5, 5 is the max page number

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
