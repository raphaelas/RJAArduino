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

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

long TIME_UNTIL_WAKEUP = 43586000L;
long ONE_MINUTE = 60000L;
long ONE_MINUTE_AFTER_WAKEUP = TIME_UNTIL_WAKEUP + ONE_MINUTE;
long ONE_DAY = 86400000L;
int DELAY_BETWEEN_REPEATS = 500;


int KEEP_BATTERY_ALIVE_LED = 13;
int STOP_ALARM_SWITCH = 2;
int NINE_SECONDS = 9000;
int BRIEF_MOMENT = 50, NOWISH = BRIEF_MOMENT;

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
  unsigned long currentMillisWithinNineSeconds = millis() % NINE_SECONDS;
  if (currentMillisWithinNineSeconds < NOWISH) {
    digitalWrite(KEEP_BATTERY_ALIVE_LED, HIGH);
    delay(BRIEF_MOMENT);
    digitalWrite(KEEP_BATTERY_ALIVE_LED, LOW);
  }
}

void loop() {
  unsigned long currentMillisWithinDay = millis() % ONE_DAY;
  keepPortableArduinoBatteryOn();
  switchState = digitalRead(STOP_ALARM_SWITCH);
  if (currentMillisWithinDay >= TIME_UNTIL_WAKEUP && currentMillisWithinDay < ONE_MINUTE_AFTER_WAKEUP) {
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {
      if (!stopPlaying) {
        if (switchState == HIGH) {
          stopPlaying = true;
        }
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(8, melody[thisNote], noteDuration);
    
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(8);
      }
    }
    delay(DELAY_BETWEEN_REPEATS);
  } else {
    stopPlaying = false;
  }

}
