#include "Arduino.h"
#include "wifi.h"
#include "button.h"
#include "display.h"
#include "datetime.h"
#include "pages.h"

void setup()
{
  Serial.begin(115200);
  wifiInit();
  buttonInit();
  displayInit();
  datetimeInit();
}

void loop()
{
  buttonCheckPress();
  pagesDisplay();
}
