#ifndef timeconstants_h
#define timeconstants_h

const int ONE_SECOND = 1000;
const int SECONDS_IN_MINUTE = 60; 
const int MINUTES_IN_HOUR = SECONDS_IN_MINUTE;
const long ONE_MINUTE = long(ONE_SECOND) * SECONDS_IN_MINUTE;
const long ONE_HOUR = ONE_MINUTE * MINUTES_IN_HOUR;
const long ONE_DAY = ONE_HOUR * 24;

#endif
