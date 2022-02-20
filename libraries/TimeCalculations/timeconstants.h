#ifndef timeconstants_h
#define timeconstants_h

#define ONE_SECOND          1000
#define SECONDS_IN_MINUTE   60
#define MINUTES_IN_HOUR     SECONDS_IN_MINUTE
const long ONE_MINUTE = long(ONE_SECOND) * SECONDS_IN_MINUTE;
const long ONE_HOUR = ONE_MINUTE * MINUTES_IN_HOUR;
const long ONE_DAY = ONE_HOUR * 24;

#endif
