#include "weekdays.h"
#include "pitches.h"
#include "notedurations.h"
#include "timeconstants.h"

const long THREE_DAYS = ONE_DAY * 3;

const int DELAY_BETWEEN_REPEATS = 500, DELAY_BETWEEN_SWITCH_LISTENS = DELAY_BETWEEN_REPEATS;
const int DELAY_DIVISOR = 10;

// This powerbank is the Arduino Powerbank 8000. In order to prevent the powerbank from
// shutting itself down after ~15 seconds of low power output, we blink a bright LED every
// 15 seconds.
const int KEEP_POWERBANK_ALIVE_COOLDOWN = ONE_SECOND * 15;
const int BRIEF_MOMENT = 50;
const int NOWISH = BRIEF_MOMENT;
const int LOW_SEVERITY_NOWISH = 5;

// These two variables should be set through an SD card over serial communication
// via the alarmClockTimeManager project however some starter values are needed.
const long STARTER_WAKEUP_TIME = ONE_MINUTE * 4;
const int STARTER_STARTING_DAY = TUESDAY;

const int ALARM_NOTE_COUNT = 8;
const int STARTUP_NOTE_COUNT = 4;

const int ALARM_NOTES[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

const int START_UP_NOTES[] = {
  NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4
};

const int ALARM_NOTE_DURATIONS[] = {
  QUARTER, EIGHTH, EIGHTH, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER
};

const int START_UP_NOTE_DURATIONS[] = {
  EIGHTH, EIGHTH, EIGHTH, EIGHTH
};

const float RECOMMENDED_NOTE_PAUSE_MULTIPLIER = 1.30;

const int MAX_COUNTDOWN = 3;
