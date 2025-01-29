#include "WifiUDP.h"
#include "NTPClient.h"
#include "Timezone.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClientSecure.h"
#include <ArduinoJson.h>

// Define NTP properties
#define NTP_OFFSET 60 * 60                       // In seconds
#define NTP_INTERVAL 60 * 1000                   // In miliseconds
#define NTP_ADDRESS "north-america.pool.ntp.org" // change this to whatever pool is closest (see ntp.org)
#define CLIENT_REFRESH_INTERVAL 5000             // Define for the refresh interval in milliseconds

// Set up the NTP UDP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *ampm[] = {"AM", "PM"};

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
};

String localTimezone = "";
DateTime dt;

unsigned long lastRefreshedAt = 0;
time_t lastRefreshedTime = 0;

void datetimeInit()
{
    timeClient.begin();
}

time_t datetimeGet()
{
    // If the current time is fresh and does not have to be refreshed
    if (lastRefreshedTime != 0 && (lastRefreshedAt + CLIENT_REFRESH_INTERVAL > millis()))
    {
        Serial.println("Returning cached time...");
        return lastRefreshedTime;
    }

    // update the NTP client and get the UNIX UTC timestamp
    Serial.println("Updating the current time...");
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();

    // convert received time stamp to time_t object
    time_t local, utc;
    utc = epochTime;

    // Then convert the UTC UNIX timestamp to local time
    TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -300};
    TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -360};
    Timezone usEastern(usEDT, usEST);
    local = usEastern.toLocal(utc);

    // Set the lastRefreshedTime and lastRefreshedAt params
    lastRefreshedTime = local;
    lastRefreshedAt = millis();

    return local;
}

String datetimeGetTimezone()
{
    // Return cached timezone
    if (localTimezone != "")
        return localTimezone;

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
            return localTimezone;
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
    return localTimezone;
}

DateTime datetimeGetTime()
{
    // Return cached time
    if (dt.isSet && dt.lastRefreshedAt + CLIENT_REFRESH_INTERVAL > millis())
        return dt;

    WiFiClientSecure client;
    HTTPClient http;

    String url = "https://timeapi.io/api/time/current/zone?timeZone=America%2FNew_York";

    client.setInsecure();
    client.connect(url, 443);

    // Get fresh time from the server
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
            return dt;
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
    else
    {
        Serial.print("Error getting local time. Code: ");
        Serial.println(responseCode);
    }

    http.end();
    return dt;
}

String datetimeGetTimeStr()
{
    datetimeGetTimezone();
    datetimeGetTime();
    String time;
    time_t local = datetimeGet();

    // format the time to 12-hour format with AM/PM and no seconds
    time += hourFormat12(local);
    time += ":";
    if (minute(local) < 10) // add a zero if minute is under 10
        time += "0";
    time += minute(local);
    time += " ";
    time += ampm[isPM(local)];

    return time;
}

String datetimeGetDateStr()
{
    String date;
    time_t local = datetimeGet();

    // now format the Time variables into strings with proper names for month, day etc
    date += days[weekday(local) - 1];
    date += ", ";
    date += months[month(local) - 1];
    date += " ";
    date += day(local);
    date += ", ";
    date += year(local);

    return date;
}