#ifndef DATETIME_H
#define DATETIME_H

#include <Arduino.h>

struct DateTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;
    String dateTime;
    String date;
    String time;
    String timezone;
    String dayOfWeek;
    bool dstActive;
    bool isSet;
    unsigned long lastRefreshedAt;

    int hourFormat12();
    uint8_t isPM();
    String getTimeStr(bool colon = true);
    String getDateStr();
    String getDatetimeStr();
    String getShortDayOfWeek();
};

void datetimeInit();
void datetimeUpdate();
void datetimeSetTimezone();
void datetimeSetTime();

#endif