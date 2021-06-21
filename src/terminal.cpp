#include <Arduino.h>
#include "terminal_setup.h"
#include "terminal_cursor.h"


bool echoHexMode = true;
uint8_t hexPosition = 0;
long hexPacketCooldownTimer = 0;


void (*cbDirty)() = nullptr;
void (*cbDebugOut)(String s) = nullptr;


//uint8_t x_cursor = 0;
//uint8_t y_cursor = 0;
uint8_t escape_sequence = 0;
uint8_t escape_command = 0;
uint8_t escape_subcommand = 0;
String escape_string = "";



void registerDirtyCallback(void (*ptr)()) {
    cbDirty = ptr;
}

void setDirtyFlag() {
    if (cbDirty) cbDirty();
}

void registerDebugOutCallback(void (*ptr)(String s)) {
    cbDebugOut = ptr;
}

void terminalDebugOut(String s) {
    if (cbDebugOut) cbDebugOut(s);
}

char textscreen[(TEXTSCREEN_WIDTH * TEXTSCREEN_HEIGHT)];



char getCharAt(uint8_t x, uint8_t y) {
    return textscreen[x + y * TEXTSCREEN_WIDTH];
}

bool toggleEchoHexMode() {
  echoHexMode = !echoHexMode;
  return echoHexMode;
}

uint8_t getScreenWidth() {
  return TEXTSCREEN_WIDTH;
}


void clearScreen() {
  for (int i = 0; i < TEXTSCREEN_HEIGHT * TEXTSCREEN_WIDTH; i++)
    textscreen[i] = ' ';
  home();
  setDirtyFlag();
}

void scrollUpScreen() {
  for (int i = 0; i < (TEXTSCREEN_HEIGHT - 1) * TEXTSCREEN_WIDTH; i++)
    textscreen[i] = textscreen[i+TEXTSCREEN_WIDTH];
  for (int i = (TEXTSCREEN_HEIGHT - 1) * TEXTSCREEN_WIDTH; i < TEXTSCREEN_HEIGHT * TEXTSCREEN_WIDTH; i++)
    textscreen[i] = ' ';
  setDirtyFlag();
}

void scrollDownScreen() {
  for (int i = TEXTSCREEN_HEIGHT * TEXTSCREEN_WIDTH - 1; i >= TEXTSCREEN_WIDTH; i--)
    textscreen[i] = textscreen[i-TEXTSCREEN_WIDTH];
  for (int i = 0; i < TEXTSCREEN_WIDTH; i++)
    textscreen[i] = ' ';
  setDirtyFlag();
}

void newLine(bool skipCarriageReturn=false) {
  if (!skipCarriageReturn) moveCursorToCol(0);
  if (getCursorPosY()+1>=TEXTSCREEN_HEIGHT) {
    scrollUpScreen();
  } else {
      cursorDown();
  }
}

void clearScreenFromTo(uint16_t startDelete, uint16_t endDelete) {
  for (int i = startDelete; i < endDelete; i++)
    textscreen[i] = ' ';
  setDirtyFlag();
}

void clearScreenFromCursorToEndOfScreen() {
  clearScreenFromTo(getCursorPosX() + getCursorPosY() * TEXTSCREEN_WIDTH, TEXTSCREEN_WIDTH * TEXTSCREEN_HEIGHT);
}

void clearScreenFromCursorToStartOfScreen() {
  clearScreenFromTo(0, getCursorPosX() + getCursorPosY() * TEXTSCREEN_WIDTH);
}

void clearLineFromCursorToEndOfLine() {
  clearScreenFromTo(getCursorPosX() + getCursorPosY() * TEXTSCREEN_WIDTH, getCursorPosY() * TEXTSCREEN_WIDTH + TEXTSCREEN_WIDTH);    
}

void clearLineFromCursorToStartOfLine() {
  clearScreenFromTo(getCursorPosY() * TEXTSCREEN_WIDTH, getCursorPosX() + getCursorPosY() * TEXTSCREEN_WIDTH);    
}

void clearLine() {
  clearScreenFromTo(getCursorPosY() * TEXTSCREEN_WIDTH, getCursorPosY() * TEXTSCREEN_WIDTH + TEXTSCREEN_WIDTH);    
}

void interpretAnsiCode(String escape_string) {
  terminalDebugOut("ANSI!\n");
  #define ANSI_MAX_ARGS 3
  int a[ANSI_MAX_ARGS] = {0, 0, 0};
  int startpos = 0;
  int pos = 0;
  int argpos = 0;
  while (pos < escape_string.length()) {
    if (String("0123456789").indexOf(escape_string.charAt(pos))!=-1) {
      // found number, all good
      a[argpos] = escape_string.substring(startpos, pos+1).toInt();
    } else if (String(",;:").indexOf(escape_string.charAt(pos))!=-1) {
      argpos++;
      startpos = pos + 1;
      if (argpos >= ANSI_MAX_ARGS) {
        // error in number of parameters - ignore this ansi code
        return;
      }
    } else if ((byte)(escape_string.charAt(pos)) > 0x40) {
      if (escape_string.charAt(pos) == 'A') { // Cursor up
        if (a[0]==0) a[0]=1;
        cursorUp(a[0]);
      }
      if (escape_string.charAt(pos) == 'B') { // Cursor down
        if (a[0]==0) a[0]=1;
        for (int i = 0; i<a[0]; i++)
          cursorDown();
      }
      if (escape_string.charAt(pos) == 'C') { // Cursor right
        if (a[0]==0) a[0]=1;
        for (int i = 0; i<a[0]; i++)
          cursorRight();
      }
      if (escape_string.charAt(pos) == 'D') { // Cursor left
        if (a[0]==0) a[0]=1;
        for (int i = 0; i<a[0]; i++)
          cursorLeft();
      }
      if (escape_string.charAt(pos) == 'E') { // Cursor next line
        if (a[0]==0) a[0]=1;
        for (int i = 0; i<a[0]; i++)
          newLine();
      }
      if (escape_string.charAt(pos) == 'F') { // Cursor previous line
        if (a[0]==0) a[0]=1;
        moveCursorToCol(0);
        for (int i = 0; i<a[0]; i++)
          cursorUp();
      }
      if (escape_string.charAt(pos) == 'G') { // Cursor horizontal absolute
        if (a[0]==0) a[0]=1;
        moveCursorToCol(a[0]-1);
      }
      if ((escape_string.charAt(pos) == 'H')||(escape_string.charAt(pos) == 'f')) { // Cursor position
        if (a[0]==0) a[0]=1;
        if (a[1]==0) a[1]=1;
        moveCursor(a[0]-1, a[1]-1);
      }
      if (escape_string.charAt(pos) == 'J') { // Erase in Display
        if (a[0]==0) {
          clearScreenFromCursorToEndOfScreen();
        }
        if (a[0]==1) {
          clearScreenFromCursorToStartOfScreen();
        }
        if (a[0]>1) {
          clearScreen();
        }
      }
      if (escape_string.charAt(pos) == 'K') { // Erase in Line
        if (a[0]==0) {
          clearLineFromCursorToEndOfLine();
        }
        if (a[0]==1) {
          clearLineFromCursorToStartOfLine();
        }
        if (a[0]>1) {
          clearLine();
        }
      }
      if (escape_string.charAt(pos) == 'S') { // Scroll up
        if (a[0]==0) a[0]=1;
        // moveCursor(x_cursor, y_cursor);
        for (int i = 0; i<a[0]; i++)
          scrollUpScreen();
      }
      if (escape_string.charAt(pos) == 'T') { // Scroll down
        if (a[0]==0) a[0]=1;
        // moveCursor(x_cursor, y_cursor);
        for (int i = 0; i<a[0]; i++)
          scrollDownScreen();
      }
    }
    pos++;
  }
}

void printCharToScreen(char c) {
//  Serial.print("-");

  uint16_t address = getCursorPosX() + TEXTSCREEN_WIDTH * getCursorPosY();
//  Serial.printf("printChar to x:%i, y:%i, addr:%i, char:%i\n", x_cursor, y_cursor, address, c);
  if (escape_sequence == 1) {
    escape_command = (byte)c;
    terminalDebugOut(" 1st byte:" + String((byte)escape_command) +"\r\n");
    escape_sequence++;
    // single command VT52
    if (escape_command == 'A') { // up
      cursorUp();
      escape_sequence = 0;
      return;
    };
    if (escape_command == 'B') { // down
      cursorDown();
      escape_sequence = 0;
      return;
    };
    if (escape_command == 'C') { // right
      cursorRight();
      escape_sequence = 0;
      return;
    };
    if (escape_command == 'D') { // left
      cursorLeft();
      escape_sequence = 0;
      return;
    };
    if (escape_command == 'E') { // not in vt52 standard
      newLine();
      escape_sequence = 0;
      return;
    }
    if (escape_command == 'H') { // home
      home();
      escape_sequence = 0;
      return;
    }
    if (escape_command == 'I') { // cursor up and insert
      if (getCursorPosY() == 0) {
        scrollDownScreen();
      } else {
        cursorUp();
      }
      escape_sequence = 0;
      return;
    }
    if (escape_command == 'J') { // clear to end of screen
      clearScreenFromCursorToEndOfScreen();
      escape_sequence = 0;
      return;
    }
    if (escape_command == 'K') { // clear to end of land
      clearLineFromCursorToEndOfLine();
      escape_sequence = 0;
      return;
    }
    if (escape_command == 'Y') { // move cursor to position
      return;
    }

    // non VT52...
    if ((byte)c == 0x9b) { // Control Sequence Introducer
      escape_string = "";
      return;
    }
    return;
  }
  if (escape_sequence == 2) {
    terminalDebugOut(" 2nd byte:" + String((byte)c) +"\r\n");
    if (escape_command == 91) {
      // VT52 Escape Codes
      if ((byte)c == 65) { // up
        cursorUp();
        escape_sequence = 0;
        return;
      };
      if ((byte)c == 66) { // down
        cursorDown();
        escape_sequence = 0;
        return;
      };
      if ((byte)c == 67) { // right
        cursorRight();
        escape_sequence = 0;
        return;
      };
      if ((byte)c == 68) { // left
        cursorLeft();
        escape_sequence = 0;
        return;
      };
      if ((byte)c == 51) { // delete (will send another byte)
        escape_subcommand = (byte)c;
        escape_sequence++;
        return;
      }
    }
    if (escape_command == 89) {
      escape_subcommand = (byte)c;
      escape_sequence++;
      return;
    }
    if (escape_command == 0x9b) {
      terminalDebugOut(" possibly ANSI \r\n");
      if (((byte)c < 0x20) || ((byte)c > 0x7e)) {
        // illegal character, so abort escape sequence
        escape_sequence = 0;
        return;
      }
      escape_string += c;
      terminalDebugOut(" added ANSI-Command: " + escape_string +"\r\n");
      if ((byte)c > 0x40) {
        interpretAnsiCode(escape_string);
        escape_sequence = 0;
      }
      return;
    }
    escape_sequence = 0;
    return;
  }
  if (escape_sequence == 3) {
    terminalDebugOut(" 3rd byte:" + String((byte)c) +"\r\n");
    if ((escape_command == 91) && (escape_subcommand == 51)) {
      if ((byte)c == 126) { // delete
        // moveCursor(x_cursor, y_cursor); // clean cursor
        for (uint8_t i = getCursorPosX(); i < TEXTSCREEN_WIDTH - 1; i++)
          textscreen[i + getCursorPosY()*TEXTSCREEN_WIDTH] = textscreen[i + getCursorPosY()*TEXTSCREEN_WIDTH+1];
        textscreen[TEXTSCREEN_WIDTH - 1 + getCursorPosY()*TEXTSCREEN_WIDTH] = ' ';
        escape_sequence = 0;
        setDirtyFlag();
        return; 
      }
    }
    if (escape_command == 89) { // move cursor
      moveCursor((byte)c-32,escape_subcommand-32);
      escape_sequence = 0;
      return;
    }
    escape_sequence = 0;
    return; 
  }
  if (((byte)c == 194) || ((byte)c == 195) || ((byte)c == 196) || ((byte)c == 197) || ((byte)c == 226)) {
    // special characters combo initial codes (unicode?)
    return;
  }
  if ((byte)c == 27) {
    terminalDebugOut("Starting Escape Sequence:\r\n");
    escape_sequence = 1;
    return;
  }
  if ((byte)c == 13) {
    moveCursorToCol(0);
    return;
  }
  if ((byte)c == 10) {
    newLine();
    return;
  }
  if ((byte)c < 32) {c = '_';} // handle unprintable unhandled chars
  textscreen[address] = c;
  //moveCursor(x_cursor+1, y_cursor);
  cursorRight();
  //if (x_cursor>=TEXTSCREEN_WIDTH) newLine();
  setDirtyFlag();
}

void printToScreen(String msg) {
  for (int i = 0; i < msg.length(); i++) {
    printCharToScreen(msg.charAt(i));
  }
}

char* getTextScreen() {
  return textscreen;
}

bool isPrintable(char c) {
  if ((c>31)&&(c<126)) return true;
  return false;
}

String toHex(char c) {
  char hex[2];
  sprintf(hex, "%02X", c);
  return (String(hex));
}

void showNextHexValue(char c) {
  long t = millis();
  if (t - HEX_PACKET_COOLDOWN_MS > hexPacketCooldownTimer) {
    hexPosition = 0;
    newLine();
    terminalDebugOut("\n\r"); 
  }
  hexPacketCooldownTimer = t;
  if (hexPosition % 8 == 0) {
    newLine();
    printToScreen(toHex(hexPosition));
    printToScreen(": ");
    terminalDebugOut("\n\r" + toHex(hexPosition) + ": "); 
  }
  moveCursorToCol(29 + (hexPosition & 0x07) +((hexPosition & 0x04)>>2));
  if (isPrintable(c)) {
    printToScreen(String(c));
  } else {
    printToScreen(".");
  }
  moveCursorToCol(4 + ((hexPosition & 0x07)*3) + ((hexPosition & 0x04)>>2));
  printToScreen(toHex(c));
  terminalDebugOut(toHex(c) + (((hexPosition & 0x03)==3) ? "  " : " ")); 
  hexPosition++;
}

void processCharacter(char c) {
  if (echoHexMode) {
    showNextHexValue(c);
  } else {
    printCharToScreen(c);
  }
  if (echoHexMode) {
  } else {
    terminalDebugOut(String(c));
  }
}

void doSetupTerminal()
{
  clearScreen();
  printToScreen("Booting up T-Console.");
  newLine(false);
  newLine(false);
}

