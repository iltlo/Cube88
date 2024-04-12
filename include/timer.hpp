#pragma once

extern byte timerMenu[7][8];

extern int hour;
extern int minute;

void initClock();

boolean getTime();

void ntpClock();

void initTimerMenu();

void showTimerMenu();
