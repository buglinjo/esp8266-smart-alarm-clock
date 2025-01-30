#include "Arduino.h"
#include "datetime.h"
#include "display.h"
#include "pages.h"

#define PAGE_REFRESH_INTERVAL 1000

int timePageLastRefreshedAt = 0;
int datePageLastRefreshedAt = 0;
bool timeColonToggle = true;
String displayedPage = "time";

void pagesShowTimePage()
{
    yield();
    if (millis() - timePageLastRefreshedAt >= PAGE_REFRESH_INTERVAL)
    {
        displayedPage = "time";
        timePageLastRefreshedAt = millis();

        String time = dt.getTimeStr(timeColonToggle);
        yield();
        if (time == "")
            return;

        timeColonToggle = !timeColonToggle;

        // Display the time
        Serial.print("Local time: ");
        Serial.println(time);
        yield();
        displayWriteTextCenter(time.c_str());
        yield();
    }
}

void pagesShowDatePage()
{
    yield();
    if (millis() - datePageLastRefreshedAt >= PAGE_REFRESH_INTERVAL)
    {
        displayedPage = "date";
        datePageLastRefreshedAt = millis();

        String date = dt.getDateStr();
        yield();
        if (date == "")
            return;

        // Display the date
        Serial.print("Local date: ");
        Serial.println(date);
        yield();
        displayWriteTextCenter(date.c_str());
        yield();
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