#include "PiezoManager.h"

PiezoManager::PiezoManager(int piezoPin) {
  this->piezoPin = piezoPin;
  this->alarmNoteCount = STARTER_ALARM_NOTE_COUNT;
  this->alarmNotes = new int[alarmNoteCount];
  this->alarmNoteDurations = new int[alarmNoteCount];
  int memorySizeOfInt = sizeof (int);
  int memorySizeOfStarterAlarmNotesArray = memorySizeOfInt * alarmNoteCount;
  memcpy(alarmNotes, STARTER_ALARM_NOTES, memorySizeOfStarterAlarmNotesArray);
  memcpy(alarmNoteDurations, STARTER_ALARM_NOTE_DURATIONS, memorySizeOfStarterAlarmNotesArray);
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
  endingNote = min(endingNote, alarmNoteCount);
  for (int note = startingNote; note < endingNote; note++) {
    if (keepSoundingAlarmClock) {
      playNote(alarmNotes[note], alarmNoteDurations[note], keepSoundingAlarmClock);
    }
  }
  return endingNote == alarmNoteCount;
}
