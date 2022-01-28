/*
  Melody

  Plays a melody

  circuit:
  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

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

long TIME_UNTIL_WAKEUP = 42279000;
long ONE_MINUTE = 60000;
int DELAY_BETWEEN_REPEATS = 500;

void setUpSerialMonitor() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("whoa");
}

void setup() {
  // no need to set up the melody.
}

void loop() {
  if (millis() >= TIME_UNTIL_WAKEUP && millis() < (TIME_UNTIL_WAKEUP + ONE_MINUTE)) {
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {
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
    delay(DELAY_BETWEEN_REPEATS);
  }

}
