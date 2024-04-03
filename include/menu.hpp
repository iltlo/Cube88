#pragma once

#include <Arduino.h>

void scrollingAnimation(const byte SRC[8], const byte DEST[8], boolean rightScrolling);

int menuOptionsRender(const byte menu[][8], int selectedPage, int maxMenuPageNum);
