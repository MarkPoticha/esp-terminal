#include <Arduino.h>
#include "WiFi.h"
#include "terminal.h"
#include "display.h"

const uint16_t keyboardPort = 23;
WiFiServer keyboardServer(keyboardPort);
WiFiClient keyboardConnection = false;
bool showKeyboardNote = true;
int echoActivationMagic = 0;
int echoHexActivationMagic = 0;
bool echoToTCP = true;

void debugOut(String s) {
  if (echoToTCP && keyboardConnection)
      if (keyboardConnection.connected())
        keyboardConnection.write(s.c_str());
}

void doSetupInputTcp(const char* ssid, const char* password) {
  long updateTimer = 0;
  printToScreen("Connecting to WiFi:");
  newLine(false);

  printToScreen("MAC: " + String(WiFi.macAddress()));
  newLine(false);
  printToScreen("SSID: " + String(ssid));
  newLine(false);
  printToScreen("Pass: ********");
  newLine(false);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    // TODO: Solange diese Phase läuft gibt es keinen Screen-Output - vielleicht mit State Machine lösen?
    if (millis() - updateTimer>=100) {
      showScreen(getTextScreen());
      updateTimer = millis();
    }
    delay(500);
    printToScreen(".");
  }
  printToScreen("OK");
  newLine(false);
  newLine(false);
  printToScreen("IP: " + WiFi.localIP().toString());
  newLine(false);
  printToScreen("RSSI: " + String(WiFi.RSSI()));
  newLine(false);
  newLine(false);
  printToScreen("Connect keyboard via TCP on port " + String(keyboardPort));
  newLine(false);
  newLine(false);
  printToScreen("Ready when you are...");
  newLine(false);
  showScreen(getTextScreen());
  keyboardServer.begin();
}

void doLoopInputTcp() {
  if (keyboardConnection) {
    if (keyboardConnection.connected()) {
      if (showKeyboardNote) {
        showKeyboardNote = false;
        debugOut("Welcome to TCP remote keyboard\n\r");
      }
      while (keyboardConnection.available()>0) {
        char c = keyboardConnection.read();
        if (c=='e') {
          echoActivationMagic++;
          if (echoActivationMagic==4) {
            echoActivationMagic = 0;
            echoToTCP = !echoToTCP;
            debugOut(String("Echo Mode: ") + (echoToTCP ? "on\n\r" : "off\n\r"));
          }
        } else {
          echoActivationMagic = 0;
        }
        if (c=='h') {
          // TODO: rüber in terminal
          echoHexActivationMagic++;
          if (echoHexActivationMagic==4) {
            echoHexActivationMagic = 0;
            bool hexMode = toggleEchoHexMode();
            debugOut(String("Echo Hex Mode: ") + (hexMode ? "on\n\r" : "off\n\r"));
          }
        } else {
          echoHexActivationMagic = 0;
        }
#ifdef USE_IR
        if (c=='i') {
          IR64BitModeMagic++;
          if (IR64BitModeMagic==4) {
            IR64BitModeMagic = 0;
            IR64BitMode = !IR64BitMode;
            debugOut(String("IR 64-Bit Mode: ") + (IR64BitMode ? "on\n\r" : "off\n\r"));
          }
        } else {
          IR64BitModeMagic = 0;
        }
#endif
        Serial.write(c);
        writeCharTemp(getScreenWidth()-1,0, String(c));
      }
//      delay(1);
    }
  } else {
    keyboardConnection = keyboardServer.available();
    showKeyboardNote = true;
  }
}