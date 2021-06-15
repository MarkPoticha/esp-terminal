#include <Arduino.h>

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

