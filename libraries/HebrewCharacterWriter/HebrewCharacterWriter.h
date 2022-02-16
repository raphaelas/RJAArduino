#ifndef hebrewcharacterwriter_h
#define hebrewcharacterwriter_h
#include "Arduino.h"
#include <LiquidCrystal.h>

struct LcdScrollData {
  bool scrollLeft;
  int scrollPositionCounter;
};

struct HoursMinutesDuration {
  int hours;
  int minutes;
};

class HebrewCharacterWriter {

public:
  HebrewCharacterWriter();
  void writeTimeLeftUntilAlarmToLcd(LiquidCrystal lcd, HoursMinutesDuration hoursMinutesDuration);
  void writeBokerTov(LiquidCrystal lcd);
  void writeSofShavuahTov(LiquidCrystal lcd);
  LcdScrollData scrollLcdMessage(LiquidCrystal lcd, LcdScrollData lcdScrollData);
  LcdScrollData resetLcdMessagePosition(LiquidCrystal lcd, LcdScrollData lcdScrollData);
private:
  void overwriteRelevantCharactersList(int newCharacters[], int relevantCharacters[]);
  int getCharacter(int characterToSearch, int relevantCharacters[]);
  LiquidCrystal createLcdSpecialCharactersForBokerTov(LiquidCrystal lcd, int relevantCharacters[]);
  LiquidCrystal createLcdSpecialCharactersForSofShavuahTov(LiquidCrystal lcd, int relevantCharacters[]);
  LiquidCrystal createLcdSpecialCharactersForTimeUntilAlarm(LiquidCrystal lcd, int relevantCharacters[]);
  void writeHey(LiquidCrystal lcd, int startingCursor, int relevantCharacters[]);
  void writeShahot(LiquidCrystal lcd, int relevantCharacters[], int startingCursor, bool singular);
  void writeDakot(LiquidCrystal lcd, int relevantCharacters[], int startingCursor, bool singular);
};
#endif
