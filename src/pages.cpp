#include "Arduino.h"
#include "datetime.h"
#include "display.h"

#define PAGE_REFRESH_INTERVAL 10000

int timePageLastRefreshedAt = 0;
int datePageLastRefreshedAt = 0;
String displayedPage = "time";

void pagesShowTimePage()
{
    if (millis() - timePageLastRefreshedAt >= PAGE_REFRESH_INTERVAL)
    {
        displayedPage = "time";
        timePageLastRefreshedAt = millis();

        String time = datetimeGetTimeStr();

        // Display the time
        Serial.print("Local time: ");
        Serial.println(time);

        displayWriteTextCenter(time.c_str());
    }
}

void pagesShowDatePage()
{
    if (millis() - datePageLastRefreshedAt >= PAGE_REFRESH_INTERVAL)
    {
        displayedPage = "date";
        datePageLastRefreshedAt = millis();
        String date = datetimeGetDateStr();

        // Display the date
        Serial.print("Local date: ");
        Serial.println(date);

        displayWriteTextCenter(date.c_str());
    }
}

String pagesGetDisplayedPage()
{
    return displayedPage;
}

bool pagesSetDisplayedPage(String page)
{
    if (page != "time" && page != "date")
        return false;

    displayedPage = page;
    timePageLastRefreshedAt = 0;
    datePageLastRefreshedAt = 0;

    return true;
}

void pagesDisplay()
{
    if (displayedPage == "time")
    {
        pagesShowTimePage();
    }
    else if (displayedPage == "date")
    {
        pagesShowDatePage();
    }
}