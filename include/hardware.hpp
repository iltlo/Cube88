#pragma once

#include <Adafruit_MPU6050.h>
#include <LedControl.h>

// Device hardware configuration
const boolean isDebug = false;
extern boolean isSilent;

const int buzzerPin = 10;

extern LedControl lc;
extern Adafruit_MPU6050 mpu;

const uint_fast8_t CMD_UP = 1;
const uint_fast8_t CMD_DOWN = 2;
const uint_fast8_t CMD_LEFT = 3;
const uint_fast8_t CMD_RIGHT = 4;


void initHardware();

void beep();
void doubleBeep();

void updateIMUData();
void readIMUData();

uint_fast8_t checkTilt();       // uses accelerometer data to check tilt
uint_fast8_t checkRotation();   // uses gyro data to check rotation
boolean isShaking();           // uses accelerometer data to check shaking
boolean isFacingDown();

void ledPrintByte(const byte pattern[]);
void ledClear();
