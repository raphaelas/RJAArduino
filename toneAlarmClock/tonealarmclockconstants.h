#include "weekdays.h"
#include "timeconstants.h"

// These two variables should be set through an SD card over serial communication
// via the alarmClockTimeManager project however some starter values are needed.
const long STARTER_WAKEUP_TIME = ONE_MINUTE * 4;
const int STARTER_STARTING_DAY = TUESDAY;

const int DELAY_BETWEEN_REPEATS = ONE_SECOND / 2, DELAY_BETWEEN_SWITCH_LISTENS = DELAY_BETWEEN_REPEATS;

// This powerbank is the Arduino Powerbank 8000. In order to prevent the powerbank from
// shutting itself down after ~15 seconds of low power output, we blink a bright LED every
// 15 seconds.
const int KEEP_POWERBANK_ALIVE_COOLDOWN = ONE_SECOND * 15;
const int COOLDOWN_BETWEEN_SERIAL_COMMUNICATION_FAILURES = ONE_SECOND * 3;

const int BRIEF_MOMENT = 50;
const int NOWISH = BRIEF_MOMENT;
const int LOW_SEVERITY_NOWISH = 5;

const char * TIME_REQUEST = "timeplease";
const char * DAY_REQUEST = "dayplease";
