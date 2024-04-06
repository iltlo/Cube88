#include <Arduino.h>

#include "hardware.hpp"
#include "matrix-pattern.hpp"
#include "menu.hpp"

// TODO: Display game score after gameover. Store highest score in EEPROM.

typedef struct Snake {
  int head[2];
  int body[40][2];
  int len;
  int dir[2];
} Snake;

typedef struct Apple {
  int rPos;
  int cPos;
} Apple;

boolean gameover = false;

byte display[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte prevDisplay[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

Snake snake = {{6, 2}, {{7, 2}, {6, 2}}, 2, {-1, 0}};
Apple apple = {(int)random(0, 8), (int)random(0, 8)};

uint_fast8_t direction;

float oldTime = 0;
float timer = 0;

float updateRate = 2.5; 

void reset() {
  for (int j = 0; j < 8; j++) {
    display[j] = 0;
  }
}

float calcDeltaTime() {
  float curTime = millis();
  float dt = curTime - oldTime;
  oldTime = curTime;
  return dt;
}

void removeFirst() {
  for (int j = 1; j < snake.len; j++) {
    snake.body[j - 1][0] = snake.body[j][0];
    snake.body[j - 1][1] = snake.body[j][1];
  }
}

void snakeUpdate() {
  reset(); // Clear the screen

  int newHead[2] = {snake.head[0] + snake.dir[0], snake.head[1] + snake.dir[1]};

  // Handle Borders
  if (newHead[0] == 8) {
    newHead[0] = 0;
  } else if (newHead[0] == -1) {
    newHead[0] = 7;
  } else if (newHead[1] == 8) {
    newHead[1] = 0;
  } else if (newHead[1] == -1) {
    newHead[1] = 7;
  }

  for (int j = 0; j < snake.len; j++) {
    if (snake.body[j][0] == newHead[0] && snake.body[j][1] == newHead[1]) {
      gameover = true;

      // Pause the game for 1 sec then Reset it
      snake = {{6, 2}, {{7, 2}, {6, 2}}, 2, {-1, 0}};  // Reinitialize the snake object
      apple = {(int)random(0, 8), (int)random(0, 8)}; // Reinitialize an apple object
      return;
    }
  }

  if (newHead[0] == apple.rPos && newHead[1] == apple.cPos) {
    snake.len = snake.len + 1;
    // Generate a new apple
    apple.rPos = (int)random(0, 8);
    apple.cPos = (int)random(0, 8);
  } else {
    removeFirst(); // Shifting the array to the left
  }

  // Update the snake's head position
  snake.body[snake.len - 1][0] = newHead[0];
  snake.body[snake.len - 1][1] = newHead[1];
  snake.head[0] = newHead[0];
  snake.head[1] = newHead[1];

  for (int j = 0; j < snake.len; j++) {
    display[snake.body[j][0]] |= 128 >> snake.body[j][1];
  }
  display[apple.rPos] |= 128 >> apple.cPos;
}

void snakeGame() {
  while (1) {
    float deltaTime = calcDeltaTime();
    timer += deltaTime;

    // direction = checkTilt();
    direction = checkRotation();

    if (direction == CMD_LEFT && snake.dir[1] == 0) {
      snake.dir[0] = 0;
      snake.dir[1] = -1;
    } else if (direction == CMD_RIGHT && snake.dir[1] == 0) {
      snake.dir[0] = 0;
      snake.dir[1] = 1;
    } else if (direction == CMD_UP && snake.dir[0] == 0) {
      snake.dir[0] = -1;
      snake.dir[1] = 0;
    } else if (direction == CMD_DOWN && snake.dir[0] == 0) {
      snake.dir[0] = 1;
      snake.dir[1] = 0;
    }

    if (timer > 1000/updateRate) {
      timer = 0;
      snakeUpdate();
    }

    // TODO: Tune gameover animation
    if (gameover) {
      ledPrintByte(emptyLED);
      delay(300);
      for (int i = 0; i < 3; i++) {
        ledPrintByte(prevDisplay);
        beep();
        delay(250);
        if (i == 2) {
          delay(600);
          scrollingAnimation(prevDisplay, emptyLED, 0);
        } else {
          ledPrintByte(emptyLED);
          delay(200);
        }
      }
      gameover = false;

      // TODO: Show game score

      break;
    }
    if (isShaking()) {
      ledPrintByte(prevDisplay);
      doubleBeep();
      delay(600);
      scrollingAnimation(prevDisplay, emptyLED, 0);
      delay(300);
      break;
    }

    ledPrintByte(display);

    for (int j = 0; j < 8; j++) {
      prevDisplay[j] = display[j];
    }
  }
}
