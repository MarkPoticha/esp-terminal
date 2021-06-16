#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREY  0x5AEB

void writeCharTemp(uint8_t x, uint8_t y, String s);
void showScreen(char* textScreen);
void doSetupDisplay();
void doLoopDisplay(char* textScreen);

