/*
  Melody alarm clock

  Plays a melody as an alarm clock

  circuit:
  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010
  modified 30 Aug 2011
  by Tom Igoe
  modified 27 Jan 2022
  by Raphael Astrow

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneMelody
*/

#include "pitches.h"
#include "weekdays.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

const long TIME_UNTIL_WAKEUP = 65375000L;
const int STARTING_WEEKDAY = SUNDAY;

const int COUNT_WEEKEND_DAYS = 2;
const int WEEKEND_DAYS[COUNT_WEEKEND_DAYS] = {SATURDAY, SUNDAY};
const int DAYS_IN_WEEK = 7;

const long ONE_MINUTE = 60000L;
const long ONE_MINUTE_AFTER_WAKEUP = TIME_UNTIL_WAKEUP + ONE_MINUTE;
const long ONE_DAY = 86400000L;

const int DELAY_BETWEEN_REPEATS = 500;
const int DELAY_DIVISOR = 10;

const int KEEP_BATTERY_ALIVE_LED = 13;
const int STOP_ALARM_SWITCH = 2;

const int KEEP_PORTABLE_BATTERY_ALIVE_COOLDOWN = 16000;
const int BRIEF_MOMENT = 50, NOWISH = BRIEF_MOMENT;

int switchState = 0;
bool stopPlaying = false;

void setUpSerialMonitor() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

void setup() {
  // no need to set up the melody.
  pinMode(KEEP_BATTERY_ALIVE_LED, OUTPUT);
  pinMode(STOP_ALARM_SWITCH, INPUT);
}

void keepPortableArduinoBatteryOn() {
  unsigned long currentMillisWithinBatteryKeepAliveCooldown = millis() % KEEP_PORTABLE_BATTERY_ALIVE_COOLDOWN;
  if (currentMillisWithinBatteryKeepAliveCooldown < NOWISH) {
    digitalWrite(KEEP_BATTERY_ALIVE_LED, HIGH);
    delay(BRIEF_MOMENT);
    digitalWrite(KEEP_BATTERY_ALIVE_LED, LOW);
  }
}

void checkStopAlarmSwitchState() {
  switchState = digitalRead(STOP_ALARM_SWITCH);
  if (switchState == HIGH) {
    stopPlaying = true;
  }
}

boolean dayIsWeekendDay(int day) {
  for (int weekendDay = 0; weekendDay < COUNT_WEEKEND_DAYS; weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == day) {
      return true;
    }
  }
  return false;
}

void splitDelayToCheckForStopPress(int delayAmount) {
  for (int i = 0; i < DELAY_DIVISOR; i++) {
    delay(delayAmount / DELAY_DIVISOR);
    checkStopAlarmSwitchState();
  }
}

void loop() {
  unsigned long currentMillisWithinDay = millis() % ONE_DAY;
  int currentDayOfWeek = STARTING_WEEKDAY + ((millis() / ONE_DAY) & DAYS_IN_WEEK);
  keepPortableArduinoBatteryOn();
  if (currentMillisWithinDay >= TIME_UNTIL_WAKEUP && currentMillisWithinDay < ONE_MINUTE_AFTER_WAKEUP
      && !dayIsWeekendDay(currentDayOfWeek)) {
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {
      checkStopAlarmSwitchState();
      if (!stopPlaying) {
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(8, melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        splitDelayToCheckForStopPress(pauseBetweenNotes);
        // stop the tone playing:
        noTone(8);
      }
    }
    splitDelayToCheckForStopPress(DELAY_BETWEEN_REPEATS);
  } else {
    stopPlaying = false;
  }
}
