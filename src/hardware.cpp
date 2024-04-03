#include <Arduino.h>
#include <LedControl.h>
#include <Adafruit_Sensor.h>

#include "hardware.hpp"

const int DIN = 6;
const int CS =  7;
const int CLK = 4;

sensors_event_t a, g, temp;
LedControl lc = LedControl(DIN, CLK, CS, 1);
Adafruit_MPU6050 mpu;
// MPU6050 imuData;

void initHardware() {
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  
  lc.shutdown(0, false);
  lc.setIntensity(0, 0);  // Set the brightness to lowest value
  lc.clearDisplay(0);

  // TODO: Add initialization animation

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  
  updateIMUData();

  Serial.println("Hardware initialized");
}


void beep() {
  digitalWrite(buzzerPin, HIGH);
  delay(30);
  digitalWrite(buzzerPin, LOW);
}

void doubleBeep() {
  beep();
  delay(40);
  beep();
}


void updateIMUData() {
  mpu.getEvent(&a, &g, &temp);
}

void readIMUData() {
  updateIMUData();

  Serial.println("====================================");
  Serial.println("Temperature (*C): " + String(temp.temperature));
  Serial.println("Acceleration (m/s^2) X: " + String(a.acceleration.x) 
                  + " Y: " + String(a.acceleration.y) 
                  + " Z: " + String(a.acceleration.z));
  Serial.println("Rotation (rad/s) X: " + String(g.gyro.x) 
                  + " Y: " + String(g.gyro.y) 
                  + " Z: " + String(g.gyro.z));
  Serial.println("====================================");
}

uint_fast8_t checkTilt() {
  updateIMUData();

  if (a.acceleration.y > 5) {
    // Serial.println("Tilted to the right");
    return CMD_RIGHT;
  } else if (a.acceleration.y < -5) {
    // Serial.println("Tilted to the left");
    return CMD_LEFT;
  } else if (a.acceleration.z < -5) {
    // Serial.println("Tilted to the top");
    return CMD_UP;
  } else if (a.acceleration.z > 5) {
    // Serial.println("Tilted to the bottom");
    return CMD_DOWN;
  } else {
    return 0;
  }
}

uint_fast8_t checkRotation() {
  updateIMUData();

  if (g.gyro.z > 4) {
    Serial.println("Rotated right");
    return CMD_RIGHT;
  } else if (g.gyro.z < -4) {
    Serial.println("Rotated left");
    return CMD_LEFT;
  } else if (g.gyro.y > 4) {
    Serial.println("Rotated up");
    return CMD_UP;
  } else if (g.gyro.y < -4) {
    Serial.println("Rotated down");
    return CMD_DOWN;
  } else {
    return 0;
  }
}


void ledPrintByte(const byte pattern[]) {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, pattern[i]);
  }
}