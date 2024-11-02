#ifndef ERTBOT_TIME_HELPERS_H
#define ERTBOT_TIME_HELPERS_H


/**
 * @brief Retrieves the current time formatted as ISO 8601.
 *
 * @return char* A string containing the current time in ISO 8601 format (UTC). The format is "YYYY-MM-DDTHH:MM:SS.000Z".
 *
 * @details This function obtains the current UTC time and formats it into an ISO 8601 string representation. The formatted time is printed
 *          to the standard output. The function uses `strftime` to format the time according to the ISO 8601 standard, and it returns a
 *          static buffer containing the formatted time.
 */
char *getCurrentEDTTimeString();

/**
 * @brief Compares two ISO 8601 formatted timestamps.
 *
 * @param time1 A string representing the first timestamp in ISO 8601 format (e.g., "YYYY-MM-DDTHH:MM:SS").
 * @param time2 A string representing the second timestamp in ISO 8601 format (e.g., "YYYY-MM-DDTHH:MM:SS").
 *
 * @return int Returns -1 if `time1` is earlier than `time2`, 1 if `time1` is later than `time2`, and 0 if both timestamps are equal.
 *
 * @details This function parses the provided ISO 8601 formatted strings into `struct tm` structures, converts these structures to `time_t`
 *          values using `mktime`, and then compares the resulting `time_t` values. It assumes the timestamps are in UTC and formatted
 *          correctly. The function adjusts year and month values to match the `struct tm` representation requirements.
 */
int compareTimes(const char* time1, const char* time2);

#endif
