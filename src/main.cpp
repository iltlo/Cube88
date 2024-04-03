#include <Arduino.h>
#include <Wifi.h>

#include "hardware.hpp"
#include "wifi-info.hpp"
#include "timer.hpp"
#include "menu.hpp"

#include "matrix-pattern.hpp"

int selectedPage = 0;

void setup() {
  Serial.begin(115200);
  initHardware();

  WiFi.begin(ssid, password);
  // connect to wifi, only wait for 5 seconds
  Serial.print("Connecting to WiFi");
  for (int i = 0; i < 5; i++) {
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("\nConnected to the WiFi network");
      break;
    }
    delay(1000);
  }
  Serial.println();

  initClock();
  
  WiFi.disconnect(true);  // disconnect from wifi after obtaining time
  WiFi.mode(WIFI_OFF);    // turn off wifi
}

void loop() {
  selectedPage = menuOptionsRender(mainMenu, selectedPage, 5);  // 0-5, 5 is the max page number
  Serial.println("Selected " + String(selectedPage));
  // doubleBeep();

  // Enter the selected menu's function
  switch(selectedPage) {
    case 0:
      // function 0
      ntpClock();
      break;
    case 1:
      // function 1
      break;
    case 2:
      // function 2
      break;
    case 3:
      // function 3
      break;
    case 4:
      // function 4
      break;
    case 5:
      // function 5
      break;
    default:
      break;
  }
  delay(250);
  
}
