#include "WifiUDP.h"
#include "NTPClient.h"
#include "Timezone.h"

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

String datetimeGetTimeStr()
{
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