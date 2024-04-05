#include <Arduino.h>

extern byte timerMenu[7][8];

extern int hour;
extern int minute;

void initClock();

void getTime();

void ntpClock();

void initTimerMenu();

void showTimerMenu();
