#ifndef musicalconstants_h
#define musicalconstants_h
#include "pitches.h"
#include "notedurations.h"

const int STARTER_ALARM_NOTE_COUNT = 9;
const int STARTUP_NOTE_COUNT = 4;

const int STARTER_ALARM_NOTES[] = {
  NOTE_C4, 0, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

const int START_UP_NOTES[] = {
  NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4
};

const int STARTER_ALARM_NOTE_DURATIONS[] = {
  EIGHTH, EIGHTH, EIGHTH, EIGHTH, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER
};

const int START_UP_NOTE_DURATIONS[] = {
  EIGHTH, EIGHTH, EIGHTH, EIGHTH
};

const float RECOMMENDED_NOTE_PAUSE_MULTIPLIER = 1.30;

#endif
