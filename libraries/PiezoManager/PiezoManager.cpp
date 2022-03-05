#include "PiezoManager.h"
#include "Arduino.h"

PiezoManager::PiezoManager(int piezoPin) {
  this->piezoPin = piezoPin;
}

void PiezoManager::playNote(int noteToPlay, int noteDuration, bool keepSoundingAlarmClock) {
  if (keepSoundingAlarmClock) {
    tone(piezoPin, noteToPlay, noteDuration);
    int pauseBetweenNotes = noteDuration * RECOMMENDED_NOTE_PAUSE_MULTIPLIER;
    delay(pauseBetweenNotes);
    noTone(piezoPin);
  }
}

void PiezoManager::playStartUpNotes() {
  for (int note = 0; note < STARTUP_NOTE_COUNT; note++) {
    playNote(START_UP_NOTES[note], START_UP_NOTE_DURATIONS[note], true);
  }
}

bool PiezoManager::soundAlarm(int startingNote, int endingNote, bool keepSoundingAlarmClock) {
  endingNote = min(endingNote, ALARM_NOTE_COUNT);
  for (int note = startingNote; note < endingNote; note++) {
    if (keepSoundingAlarmClock) {
      playNote(ALARM_NOTES[note], ALARM_NOTE_DURATIONS[note], keepSoundingAlarmClock);
    }
  }
  return endingNote == ALARM_NOTE_COUNT;
}
