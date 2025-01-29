#include "Arduino.h"
#include "wifi.h"
#include "pages.h"
#include "display.h"

#define BUTTON_PIN D0
#define BUTTON_SHORT_PRESS_TIME 10
#define BUTTON_LONG_PRESS_TIME 3000

unsigned long pressStart = 0;

void buttonInit()
{
    pinMode(BUTTON_PIN, INPUT);
}

void buttonCheckPress()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        if (pressStart == 0)
            pressStart = millis();

        if (millis() - pressStart > BUTTON_LONG_PRESS_TIME)
        {
            Serial.println("________________LONG PRESS DETECTED________________");
            displayShowMainLogo();
            ESP.restart();
            // wifiReset();
            pressStart = 0;
        }
    }
    else if (pressStart != 0)
    {
        unsigned long pressDuration = millis() - pressStart;
        if (pressDuration > BUTTON_SHORT_PRESS_TIME && pressDuration < BUTTON_LONG_PRESS_TIME)
        {
            if (pagesGetDisplayedPage() == "time")
                pagesSetDisplayedPage("date");
            else
                pagesSetDisplayedPage("time");
        }
        Serial.print("Press duration:");
        Serial.print(millis() - pressStart);
        Serial.println(" ms");
        pressStart = 0;
    }
}