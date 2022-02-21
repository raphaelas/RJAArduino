#include "weekdays.h"
#include "timeconstants.h"
#include "tonealarmclockstructs.h"

// These two variables should be set through an SD card over serial communication
// via the alarmClockTimeManager project however some starter values are needed.
const long STARTER_WAKEUP_TIME = ONE_MINUTE * 4;
const int STARTER_STARTING_DAY = TUESDAY;

struct LcdScrollData lcdScrollData = {true, 0};

const int DELAY_BETWEEN_REPEATS = 500, DELAY_BETWEEN_SWITCH_LISTENS = DELAY_BETWEEN_REPEATS;
const int DELAY_DIVISOR = 10;

// This powerbank is the Arduino Powerbank 8000. In order to prevent the powerbank from
// shutting itself down after ~15 seconds of low power output, we blink a bright LED every
// 15 seconds.
const int KEEP_POWERBANK_ALIVE_COOLDOWN = ONE_SECOND * 15;
const int ALARM_SOUNDING_POWERBANK_MAX_COUNTDOWN = 3;

const int BRIEF_MOMENT = 50;
const int NOWISH = BRIEF_MOMENT;
const int LOW_SEVERITY_NOWISH = 5;
