#include "WifiUDP.h"
#include "NTPClient.h"
#include "Timezone.h"

// Define NTP properties
#define NTP_OFFSET 60 * 60                       // In seconds
#define NTP_INTERVAL 60 * 1000                   // In miliseconds
#define NTP_ADDRESS "north-america.pool.ntp.org" // change this to whatever pool is closest (see ntp.org)

// Set up the NTP UDP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *ampm[] = {"AM", "PM"};

void datetimeInit()
{
    timeClient.begin();
}

time_t datetimeGet()
{
    // update the NTP client and get the UNIX UTC timestamp
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();

    // convert received time stamp to time_t object
    time_t local, utc;
    utc = epochTime;

    // Then convert the UTC UNIX timestamp to local time
    TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -300}; // UTC - 5 hours - change this as needed
    TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -360};  // UTC - 6 hours - change this as needed
    Timezone usEastern(usEDT, usEST);
    local = usEastern.toLocal(utc);

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