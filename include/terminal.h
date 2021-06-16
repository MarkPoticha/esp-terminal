

void setCursorPos(uint8_t newCursorX, uint8_t newCursorY);
void getCursorPos(uint8_t* currentCursorX, uint8_t* currentCursorY);
uint8_t getCursorPosX();
uint8_t getCursorPosY();
char getCharAt(uint8_t x, uint8_t y);
void printToScreen(String msg);
void newLine(bool skipCarriageReturn);
bool toggleEchoHexMode();
uint8_t getScreenWidth();
char* getTextScreen();
void registerDirtyCallback(void (*ptr)());
void setDirtyFlag();
void registerDebugOutCallback(void (*ptr)(String s));
void terminalDebugOut(String s);
void clearScreen();
void processCharacter(char c);