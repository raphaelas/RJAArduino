#ifndef hebrewcharacterwriter_h
#define hebrewcharacterwriter_h
#include "Arduino.h"
#include <LiquidCrystal.h>

struct LcdScrollData;
struct HoursMinutesDuration;

class HebrewCharacterWriter {

  public:
    HebrewCharacterWriter(int lcd_rs_pin, int lcd_e_pin, int lcd_d4_pin,
    int lcd_d5_pin, int lcd_d6_pin, int lcd_d7_pin);
    void writeTimeLeftUntilAlarmToLcd(HoursMinutesDuration hoursMinutesDuration);
    void writeBokerTov();
    void writeSofShavuahTov();
    void writeChagSameach(int dayNumber);
    LcdScrollData scrollLcdMessage(LcdScrollData lcdScrollData);
    LcdScrollData resetLcdMessagePosition(LcdScrollData lcdScrollData);
  private:
    void overwriteRelevantCharactersList(int newCharacters[], int relevantCharacters[]);
    int getCharacter(int characterToSearch, int relevantCharacters[]);
    void createLcdSpecialCharactersForBokerTov(int relevantCharacters[]);
    void createLcdSpecialCharactersForSofShavuahTov(int relevantCharacters[]);
    void createLcdSpecialCharactersForChagSameach(int relevantCharacters[]);
    void createLcdSpecialCharactersForTimeUntilAlarm(int relevantCharacters[]);
    void writeHey(int startingCursor, int relevantCharacters[]);
    void writeShahot(int relevantCharacters[], int startingCursor, bool singular);
    void writeDakot(int relevantCharacters[], int startingCursor, bool singular);
    void writeDayNumber(int relevantCharacters[], int dayNumber);
    LiquidCrystal * lcd;
};
#endif
