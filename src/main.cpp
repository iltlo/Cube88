#include <Arduino.h>
#include <Wifi.h>

#include "hardware.hpp"
#include "wifi-info.hpp"
#include "timer.hpp"

#include "matrix-pattern.hpp"

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
  readIMUData();
  checkRotation();

  getTime();

  static int count = 0;
  ledPrintByte(numbers[count]);

  count = (count + 1) % 101;
  delay(200);
}
