#ifndef piezomanager_h
#define piezomanager_h
#include "Arduino.h"
#include "musicalconstants.h"

class PiezoManager {
  public:
    PiezoManager(int piezoPin);
    void playNote(int noteToPlay, int noteDuration, bool keepSoundingAlarmClock);
    void playStartUpNotes();
    bool soundAlarm(int startingNote, int endingNote, bool keepSoundingAlarmClock);
  private:
    int piezoPin;
    int alarmNoteCount;
    int * alarmNotes;
    int * alarmNoteDurations;
};

#endif
