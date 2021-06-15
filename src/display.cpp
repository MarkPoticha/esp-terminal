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

// temporary screen write - will be overwritten by next full refresh
void writeCharTemp(uint8_t x, uint8_t y, String s) {
  tft.setCursor(x*6, y*8);
  tft.print(s);
}

void toggleCursor() {
  uint8_t x, y;
  getCursorPos(&x, &y);
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

void showScreen(char* textScreen, bool screenDirty) {
//  tft.setTextWrap(true); 
//  tft.setTextDatum(TL_DATUM);
  if (screenDirty) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(0,0);
    tft.print(String(textScreen));
    screenDirty = false;
  }
}

void doSetupDisplay() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(BLACK);
}

void doLoopDisplay(char* textScreen, bool screenDirty) {
  if (millis() - updateTimer>=20) {
    showScreen(textScreen, screenDirty);
    updateTimer = millis();
  }
  if (millis() - cursorTimer>=500) {
    toggleCursor();
    cursorTimer = millis();
  }
}