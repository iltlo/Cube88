#include <Arduino.h>

#include "hardware.hpp"
#include "matrix-pattern.hpp"

// general function for scrolling animation
void scrollingAnimation(const byte SRC[8], const byte DEST[8], boolean rightScrolling) {
  byte menuWindow[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  for (int i = 0; i < 8; i++) {
    byte byteValue;
    for (int j = 0; j < 8; j++) {   // each row
      if (rightScrolling) {
        byteValue = SRC[j] << i +1;
        byteValue = byteValue | (DEST[j] >> 8-i - 1);
      } else {
        byteValue = SRC[j] >> i +1;
        byteValue = (DEST[j] << 8-i - 1) | byteValue;  
      }
      menuWindow[j] = byteValue;
    }
    ledPrintByte(menuWindow);
    
    delay(25);
    byte menuWindow[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  }
  // delay(200);
}

// general function for showing menu options
int menuOptionsRender(const byte menu[][8], int selectedPage, int maxMenuPageNum) {
  int curPageNum = selectedPage, destPageNum = selectedPage;
  const byte *SRC_MENU = menu[curPageNum];
  const byte *DEST_MENU;
  boolean rightScrolling = 1;  // 1 for right, 0 for left
  uint_fast8_t direction;

  long previousMillis = 0;
  unsigned long currentMillis;
  boolean startedCounting = false;

  while (1) {
    ledPrintByte(SRC_MENU);
    direction = checkRotation();

    if (direction == CMD_RIGHT || direction == CMD_LEFT) {
      destPageNum = (direction == CMD_RIGHT) ? (curPageNum + 1) % (maxMenuPageNum + 1) : 
                                    (curPageNum == 0) ? maxMenuPageNum : curPageNum - 1;
      if (isDebug) Serial.println("curPageNum: " + String(curPageNum) + ", destPageNum: " + String(destPageNum) + ", Max: " + String(maxMenuPageNum));
      rightScrolling = (direction == CMD_RIGHT);
      beep();

      DEST_MENU = menu[destPageNum];
      scrollingAnimation(SRC_MENU, DEST_MENU, rightScrolling);
      delay(150);

      SRC_MENU = DEST_MENU;
      curPageNum = destPageNum;
    }

    direction = checkRotation();
    if (direction == CMD_UP) {
      return curPageNum;
    }

    if (isShaking()) {
      return -1;
    }

    if (isFacingDown()) {
      currentMillis = millis();
      if (currentMillis - previousMillis >= 2000) {   // interval of 2 seconds
        if (startedCounting) {
          if (isDebug) Serial.println("Going to sleep");
          longBeep();
          ledPrintByte(emptyLED);
          while (isFacingDown()) {
            esp_sleep_enable_timer_wakeup(5 * 1000000); // 5 second
            esp_light_sleep_start();

            if (isDebug) Serial.println("Waking up");
            if (!isFacingDown()) {
              startedCounting = false;
              break;
            }
          }
          doubleBeep();
        } else {
          startedCounting = true;
          previousMillis = currentMillis;
        }
      }
    }
  }
  delay(150);
}
