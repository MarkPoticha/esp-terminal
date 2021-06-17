#include <Arduino.h>
#include "terminal.h"

#define USE_IR_ 1

#ifdef USE_IR

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#define RX_PIN 26
#define TX_PIN 25
bool IR64BitMode = true;
int IR64BitModeMagic = 0;

IRrecv irrecv(RX_PIN);
decode_results results;

#endif

void doSetupSerial()
{
    #ifdef USE_IR
        Serial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    #else
        Serial.begin(115200);
    #endif
    delay(20);
    Serial.println("Starting...");
}
void doSetupIR()
{
    #ifdef USE_IR
        irrecv.enableIRIn();  // Start the IR receiver
        printToScreen("IRMode: enabled");
        newLine();
    #endif
}

void doLoopSerial()
{
#ifdef USE_IR
  if (irrecv.decode(&results)) {
    uint64_t v = results.value;
    if (IR64BitMode) {
      for (int i=7; i>=4; i--) {
        processCharacter((v >> (8*i)) & 0xff);
      }
    }
    for (int i=3; i>=0; i--) {
      processCharacter((v >> (8*i)) & 0xff);
    }
    irrecv.resume();      
  }
#else
  if (Serial.available()>0) {
    char c = Serial.read();
    processCharacter(c);
 //   Serial.printf("cursor at x:%i, y:%i\n, last char:%i", x_cursor, y_cursor, (byte)c);
  }
#endif
}