#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "terminal.h"
#include "display.h"

#define CURSOR '_'

// TODO: tft-Objekt erst anlegen in doSetupDisplay()
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
long updateTimer = 0;
long cursorTimer = 0;
bool cursorBlink = false;
uint8_t oldCursorX = 0;
uint8_t oldCursorY = 0;
bool displayScreenDirty = true;

// temporary screen write - will be overwritten by next full refresh
void writeCharTemp(uint8_t x, uint8_t y, String s) {
  tft.setCursor(x*6, y*8);
  tft.print(s);
}

void toggleCursor() {
  uint8_t x, y;
//  getCursorPos(&x, &y);
  x = getCursorPosX();
  y = getCursorPosY();
  if (cursorBlink) {
    tft.setCursor(x*6, y*8);
    tft.print(CURSOR);
  } else {
    tft.setCursor(oldCursorX*6, oldCursorY*8);
    tft.print(getCharAt(oldCursorX, oldCursorY));
  }
  cursorBlink = !cursorBlink;
  oldCursorX = x;
  oldCursorY = y;
} 

void showScreen(char* textScreen) {
//  tft.setTextWrap(true); 
//  tft.setTextDatum(TL_DATUM);
  if (displayScreenDirty) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(0,0);
    tft.print(String(textScreen));
    displayScreenDirty = false;
  }
}

void setDisplayScreenDirty() {
  displayScreenDirty = true;
}

void doSetupDisplay() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  registerDirtyCallback(setDisplayScreenDirty);
}

void doLoopDisplay(char* textScreen) {
  if (millis() - updateTimer>=20) {
    showScreen(textScreen);
    updateTimer = millis();
  }
  if (millis() - cursorTimer>=500) {
    toggleCursor();
    cursorTimer = millis();
  }
}