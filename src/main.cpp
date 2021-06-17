//#include <Arduino.h>

#include "wifi-login.h"
#include "input_tcp.h"
#include "terminal.h"
#include "display.h"
#include "serial.h"

void setup() {
  doSetupSerial();
  doSetupDisplay();
  doSetupTerminal();
  doSetupInputTcp(ssid, password);
  doSetupIR();
}


void loop() {
  doLoopDisplay(getTextScreen());
  doLoopInputTcp();
  doLoopSerial();
}