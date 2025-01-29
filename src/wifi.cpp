#include "WiFiManager.h"

#define AP_SSID "B&I Alarm"
#define AP_PASS "123123123"

WiFiManager wm;

void wifiInit()
{
    std::vector<const char *> menu = {"wifi", "info"};

    wm.setMenu(menu);
    wm.setShowInfoUpdate(false);
    wm.setClass("invert");

    bool res;
    res = wm.autoConnect(AP_SSID, AP_PASS); // password protected ap

    if (!res)
    {
        Serial.println("Failed to connect or hit timeout");
        // ESP.restart();
    }
    else
    {
        // if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
    }
}

void wifiReset()
{
    Serial.println("Erasing Config, restarting");
    wm.resetSettings();
    ESP.restart();
}