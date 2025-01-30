#include "datetime.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClientSecure.h"
#include <ArduinoJson.h>

#define TIME_UPDATE_INTERVAL 10000 // Define for the time update interval in milliseconds

const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *ampm[] = {"AM", "PM"};

String localTimezone = "";
DateTime dt;

int DateTime::hourFormat12()
{
    if (hour == 0)
        return 12; // 12 midnight
    else if (hour > 12)
        return hour - 12;
    else
        return hour;
}

uint8_t DateTime::isPM()
{
    return (hour >= 12);
}

String DateTime::getTimeStr(bool colon)
{
    if (!isSet)
        return "";

    String time;
    time += hourFormat12();
    if (colon)
        time += ":";
    else
        time += " ";

    if (minute < 10)
        time += "0";
    time += minute;
    time += " ";
    time += ampm[isPM()];

    return time;
}

String DateTime::getDateStr()
{
    if (!isSet)
        return "";

    String date;
    date += months[month - 1];
    date += " ";
    date += day;
    date += ", ";
    date += year;

    return date;
}

String DateTime::getDatetimeStr()
{
    return date + " " + time;
}

void datetimeInit()
{
    datetimeSetTimezone();
}

void datetimeUpdate()
{
    unsigned long currentMillis = millis();
    if (currentMillis - dt.lastRefreshedAt >= TIME_UPDATE_INTERVAL)
    {
        dt.lastRefreshedAt = currentMillis;
        datetimeSetTime();
    }
}

void datetimeSetTimezone()
{
    // Return cached timezone
    if (localTimezone != "")
        return;

    WiFiClient client;
    HTTPClient http;

    // Get timezone details
    String url = "http://ip-api.com/json";
    http.begin(client, url.c_str());
    int responseCode = http.GET();

    if (responseCode == 200)
    {
        String body = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, body);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        localTimezone = doc["timezone"].as<String>();

        Serial.print("Got Timezone: ");
        Serial.println(localTimezone);
    }
    else
    {
        Serial.print("Error getting timezone. Code: ");
        Serial.println(responseCode);
    }

    http.end();
}

void handleTimeUpdate(String body)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    dt.year = doc["year"];
    dt.month = doc["month"];
    dt.day = doc["day"];
    dt.hour = doc["hour"];
    dt.minute = doc["minute"];
    dt.second = doc["seconds"];
    dt.millisecond = doc["milliSeconds"];
    dt.dateTime = doc["dateTime"].as<String>();
    dt.date = doc["date"].as<String>();
    dt.time = doc["time"].as<String>();
    dt.timezone = doc["timezone"].as<String>();
    dt.dayOfWeek = doc["dayOfWeek"].as<String>();
    dt.dstActive = doc["dstActive"];
    dt.isSet = true;
    dt.lastRefreshedAt = millis();

    Serial.println("----------------- Got time ----------------");
    Serial.print("year: ");
    Serial.println(dt.year);
    Serial.print("month: ");
    Serial.println(dt.month);
    Serial.print("day: ");
    Serial.println(dt.day);
    Serial.print("hour: ");
    Serial.println(dt.hour);
    Serial.print("minute: ");
    Serial.println(dt.minute);
    Serial.print("second: ");
    Serial.println(dt.second);
    Serial.print("millisecond: ");
    Serial.println(dt.millisecond);
    Serial.print("dateTime: ");
    Serial.println(dt.dateTime);
    Serial.print("date: ");
    Serial.println(dt.date);
    Serial.print("time: ");
    Serial.println(dt.time);
    Serial.print("timezone: ");
    Serial.println(dt.timezone);
    Serial.print("dayOfWeek: ");
    Serial.println(dt.dayOfWeek);
    Serial.print("lastRefreshedAt: ");
    Serial.println(dt.lastRefreshedAt);
    Serial.println("----------------- END time ----------------");
}

void datetimeSetTime()
{
    WiFiClientSecure client;
    HTTPClient http;

    String url = "https://timeapi.io/api/time/current/zone?timeZone=" + localTimezone;

    client.setInsecure();
    client.connect(url, 443);

    // Get fresh time from the server
    http.begin(client, url.c_str());
    int responseCode = http.GET();

    if (responseCode == 200)
    {
        handleTimeUpdate(http.getString());
    }
    else
    {
        Serial.print("Error getting local time. Code: ");
        Serial.println(responseCode);
    }

    http.end();
}