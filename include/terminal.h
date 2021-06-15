

void setCursorPos(uint8_t newCursorX, uint8_t newCursorY);
void getCursorPos(uint8_t* currentCursorX, uint8_t* currentCursorY);
char getCharAt(uint8_t x, uint8_t y);
void printToScreen(String msg);
void newLine(bool skipCarriageReturn);
bool toggleEchoHexMode();
uint8_t getScreenWidth();
char* getTextScreen();

