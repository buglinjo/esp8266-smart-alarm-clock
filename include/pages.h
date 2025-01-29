#ifndef PAGES_H
#define PAGES_H

#include "Arduino.h"

void pagesShowTimePage();
void pagesShowDatePage();
String pagesGetDisplayedPage();
bool pagesSetDisplayedPage(String page);
void pagesDisplay();

#endif