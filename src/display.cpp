#include "Adafruit_SSD1306.h"
#include "images.cpp"

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixe
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // < See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayInit()
{
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay();
    display.drawBitmap(48, 0, imagesStartupLogo, 32, 32, 1);
    display.display();
}

void displayShowMainLogo()
{
    display.clearDisplay();
    display.drawBitmap(48, 0, imagesStartupLogo, 32, 32, 1);
    display.display();
}

void displayWriteTextCenter(const char *s)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(0);

    int16_t x, y;
    uint16_t w, h;
    display.getTextBounds(s, 0, 0, &x, &y, &w, &h);

    if (w * 2 <= SCREEN_WIDTH)
        display.setTextSize(2);
    else
        display.setTextSize(1);

    display.getTextBounds(s, 0, 0, &x, &y, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
    display.cp437(true);

    display.write(s);
    display.display();
}