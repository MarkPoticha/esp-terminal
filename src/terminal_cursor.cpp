#include <Arduino.h>
#include "terminal_setup.h"
// TODO: get rid of this
#include "terminal.h"

uint8_t cursorX = 0; 
uint8_t cursorY = 0;

void setCursorPos(uint8_t newCursorX, uint8_t newCursorY) {
    cursorX = newCursorX;
    cursorY = newCursorY;
}

void getCursorPos(uint8_t* currentCursorX, uint8_t* currentCursorY) {
    currentCursorX = &cursorX;
    currentCursorY = &cursorY;
}

uint8_t getCursorPosX() {
    return cursorX;
}

uint8_t getCursorPosY() {
    return cursorY;
}

void moveCursor(uint8_t newX, uint8_t newY) {
  cursorX = newX;
  cursorY = newY;
}

void home() {
  moveCursor(0,0);
}

void moveCursorToCol(uint8_t newX) {
  moveCursor(newX, cursorY);
}

void moveCursorToRow(uint8_t newY) {
  moveCursor(cursorX, newY);
}

void cursorUp() {
  if (cursorY > 0) moveCursor(cursorX, cursorY - 1);
}

void cursorUp(uint8_t count) {
  moveCursor(cursorX, cursorY - min(cursorY, count));
}

void cursorDown() {
  if (cursorY < (TEXTSCREEN_HEIGHT-1)) {
    moveCursor(cursorX, cursorY + 1);
  } else {
    newLine(true);
  }
}

void cursorRight() {
  if (cursorX < TEXTSCREEN_WIDTH) {
    moveCursor(cursorX + 1, cursorY);
  } else {
    newLine(false);
  }
}

void cursorLeft() {
  if (cursorX > 0) {
    moveCursor(cursorX - 1, cursorY);
  } else {
    if (cursorY > 0) moveCursor(TEXTSCREEN_WIDTH-1, cursorY - 1);
  }
}

