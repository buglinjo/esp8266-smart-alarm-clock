#ifndef PAGES_H
#define PAGES_H

#include "Arduino.h"
#include "datetime.h"

extern DateTime dt;

void pagesShowTimePage();
void pagesShowDatePage();
String pagesGetDisplayedPage();
bool pagesSetDisplayedPage(String page);
void pagesDisplay();

#endif