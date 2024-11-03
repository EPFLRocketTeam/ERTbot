#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "ERTbot_common.h"

char* getCurrentEDTTimeString() {
    log_message(LOG_DEBUG, "Entering function getCurrentEDTTimeString");

    // Allocate static memory for the ISO 8601 string
    static char iso8601[26];  // Size 26 for "YYYY-MM-DDTHH:MM:SS-04:00\0"

    // Set the timezone to Eastern Time
    setenv("TZ", "GMT", 1);
    tzset();

    // Get the current time
    time_t now = time(NULL);

    // Convert to local time (in EDT)
    const struct tm *edtTime = localtime(&now);

    // Check if we are in Daylight Saving Time (EDT)
    if (edtTime->tm_isdst > 0) {
        // EDT timezone is UTC-4
        strftime(iso8601, sizeof(iso8601), "%Y-%m-%dT%H:%M:%S-04:00", edtTime);
    } else {
        // If not DST, fallback to EST (UTC-5)
        strftime(iso8601, sizeof(iso8601), "%Y-%m-%dT%H:%M:%S-05:00", edtTime);
    }


    log_message(LOG_DEBUG, "Exiting function getCurrentEDTTimeString");
    return iso8601;
}

int compareTimes(const char* time1, const char* time2) {
    log_message(LOG_DEBUG, "Entering function compareTimes");

    struct tm tm1;
    struct tm tm2;
    memset(&tm1, 0, sizeof(struct tm));
    memset(&tm2, 0, sizeof(struct tm));

    // Parse time1
    sscanf(time1, "%d-%d-%dT%d:%d:%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday,
                                         &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
    tm1.tm_year -= 1900;  // Years since 1900
    tm1.tm_mon--;         // Months are 0-11 in struct tm

    // Parse time2
    sscanf(time2, "%d-%d-%dT%d:%d:%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday,
                                         &tm2.tm_hour, &tm2.tm_min, &tm2.tm_sec);
    tm2.tm_year -= 1900;  // Years since 1900
    tm2.tm_mon--;         // Months are 0-11 in struct tm

    // Convert struct tm to time_t
    time_t time1_t = mktime(&tm1);
    time_t time2_t = mktime(&tm2);

    // Compare time_t values
    if (time1_t < time2_t){
        log_message(LOG_DEBUG, "Exiting function compareTimes");
        return -1;
    }
    else if (time1_t > time2_t){

        log_message(LOG_DEBUG, "Exiting function compareTimes");
        return 1;
    }
    else{

        log_message(LOG_DEBUG, "Exiting function compareTimes");
        return 0;
    }
}
