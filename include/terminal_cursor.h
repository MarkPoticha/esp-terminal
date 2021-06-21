#include <Arduino.h>

void setCursorPos(uint8_t newCursorX, uint8_t newCursorY);
void getCursorPos(uint8_t* currentCursorX, uint8_t* currentCursorY);
uint8_t getCursorPosX();
uint8_t getCursorPosY();
void moveCursor(uint8_t newX, uint8_t newY);
void moveCursorToRow(uint8_t newY);
void home();
void moveCursorToCol(uint8_t newX);
void cursorUp();
void cursorUp(uint8_t count);
void cursorDown();
void cursorRight();
void cursorLeft();