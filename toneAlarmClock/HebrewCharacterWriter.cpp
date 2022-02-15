#include "Arduino.h"
#include "HebrewCharacterWriter.h"
#include "hebrewcharacters.h"

const int MAX_SCROLL_AMOUNT = 3;
const int RELEVANT_CHARACTERS_COUNT = 8;

HebrewCharacterWriter::HebrewCharacterWriter() {}

void HebrewCharacterWriter::writeTimeLeftUntilAlarmToLcd(LiquidCrystal lcd, HoursMinutesDuration hoursMinutesDuration) {
  int hoursLeftUntilAlarm = hoursMinutesDuration.hours;
  int minutesLeftUntilAlarm = hoursMinutesDuration.minutes;
  int relevantCharacters[RELEVANT_CHARACTERS_COUNT];
  lcd = createLcdSpecialCharactersForTimeUntilAlarm(lcd, relevantCharacters);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(14, 0);
  int minutesCursor;
  if (hoursLeftUntilAlarm > 0) {
    lcd.print(hoursLeftUntilAlarm);
    bool singularHours = (hoursLeftUntilAlarm == 1);
    writeShahot(lcd, relevantCharacters, 12, singularHours);
    lcd.setCursor(6 + singularHours, 0);
    lcd.print(minutesLeftUntilAlarm);
    writeDakot(lcd, relevantCharacters, 4 + singularHours, minutesLeftUntilAlarm == 1);
  } else {
    lcd.print(minutesLeftUntilAlarm);
    writeDakot(lcd, relevantCharacters, 12, minutesLeftUntilAlarm == 1);
  }
}

void HebrewCharacterWriter::writeBokerTov(LiquidCrystal lcd) {
  int relevantCharacters[RELEVANT_CHARACTERS_COUNT];
  lcd = createLcdSpecialCharactersForBokerTov(lcd, relevantCharacters);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(15, 0);
  lcd.write(getCharacter(BET, relevantCharacters));
  lcd.setCursor(14, 0);
  lcd.write(getCharacter(VAV, relevantCharacters));
  lcd.setCursor(13, 0);
  lcd.write(getCharacter(KUF, relevantCharacters));
  lcd.setCursor(12, 0);
  lcd.write(getCharacter(RESH, relevantCharacters));
  lcd.setCursor(10, 0);
  lcd.write(getCharacter(TET, relevantCharacters));
  lcd.setCursor(9, 0);
  lcd.write(getCharacter(VAV, relevantCharacters));
  lcd.setCursor(8, 0);
  lcd.write(getCharacter(BET, relevantCharacters));
  lcd.setCursor(6, 0);
  lcd.write(getCharacter(RESH, relevantCharacters));
  lcd.setCursor(5, 0);
  lcd.write(getCharacter(FEY, relevantCharacters));
  lcd.setCursor(4, 0);
  lcd.write(getCharacter(ALEPH, relevantCharacters));
  lcd.setCursor(3, 0);
  lcd.write(getCharacter(LAMED, relevantCharacters));
}

void HebrewCharacterWriter::writeSofShavuahTov(LiquidCrystal lcd) {
  int relevantCharacters[RELEVANT_CHARACTERS_COUNT];
  lcd = createLcdSpecialCharactersForSofShavuahTov(lcd, relevantCharacters);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(15, 0);
  lcd.write(getCharacter(SAMECH, relevantCharacters));
  lcd.setCursor(14, 0);
  lcd.write(getCharacter(VAV, relevantCharacters));
  lcd.setCursor(13, 0);
  lcd.write(getCharacter(FEYSOFIT, relevantCharacters));
  lcd.setCursor(11, 0);
  lcd.write(getCharacter(SHIN, relevantCharacters));
  lcd.setCursor(10, 0);
  lcd.write(getCharacter(BET, relevantCharacters));
  lcd.setCursor(9, 0);
  lcd.write(getCharacter(VAV, relevantCharacters));
  lcd.setCursor(8, 0);
  lcd.write(getCharacter(AYIN, relevantCharacters));
  lcd.setCursor(6, 0);
  lcd.write(getCharacter(TET, relevantCharacters));
  lcd.setCursor(5, 0);
  lcd.write(getCharacter(VAV, relevantCharacters));
  lcd.setCursor(4, 0);
  lcd.write(getCharacter(BET, relevantCharacters));
}

LcdScrollData HebrewCharacterWriter::scrollLcdMessage(LiquidCrystal lcd, LcdScrollData lcdScrollData) {
  bool scrollLeft = lcdScrollData.scrollLeft;
  int scrollPositionCounter = lcdScrollData.scrollPositionCounter;
  if (scrollPositionCounter == MAX_SCROLL_AMOUNT) {
    scrollLeft = !scrollLeft;
    scrollPositionCounter = 0;
  }
  if (scrollLeft) {
    lcd.scrollDisplayLeft();
  } else {
    lcd.scrollDisplayRight();
  }
  scrollPositionCounter++;
  return (LcdScrollData) {scrollLeft, scrollPositionCounter};
}

LcdScrollData HebrewCharacterWriter::resetLcdMessagePosition(LiquidCrystal lcd, LcdScrollData lcdScrollData) {
  bool scrollLeft = lcdScrollData.scrollLeft;
  int scrollPositionCounter = lcdScrollData.scrollPositionCounter;
  if (!scrollLeft) {
    scrollPositionCounter = MAX_SCROLL_AMOUNT - scrollPositionCounter;
  }
  while (scrollPositionCounter > 0) {
    lcd.scrollDisplayRight();
    scrollPositionCounter--;
  }
  scrollLeft = true;
  return (LcdScrollData) {scrollLeft, scrollPositionCounter};
}

void HebrewCharacterWriter::overwriteRelevantCharactersList(int newCharacters[], int relevantCharacters[]) {
  for (int i = 0; i < RELEVANT_CHARACTERS_COUNT; i++) {
    relevantCharacters[i] = newCharacters[i];
  }
}

int HebrewCharacterWriter::getCharacter(int characterToSearch, int relevantCharacters[]) {
  for (int i = 0; i < RELEVANT_CHARACTERS_COUNT; i++) {
    if (characterToSearch == relevantCharacters[i]) {
      return i;
    }
  }
  return -1;
}

int HebrewCharacterWriter::setCharacter(int characterToSearch, int relevantCharacters[]) {
  for (int i = 0; i < RELEVANT_CHARACTERS_COUNT; i++) {
    if (characterToSearch == relevantCharacters[i]) {
      return i;
    }
  }
  return -1;
}

LiquidCrystal HebrewCharacterWriter::createLcdSpecialCharactersForBokerTov(LiquidCrystal lcd, int relevantCharacters[]) {
  int newCharacters[] = {BET, VAV, KUF, RESH, TET, FEY, ALEPH, LAMED};
  overwriteRelevantCharactersList(newCharacters, relevantCharacters);
  lcd.createChar(setCharacter(BET, relevantCharacters), bet);
  lcd.createChar(setCharacter(VAV, relevantCharacters), vav);
  lcd.createChar(setCharacter(KUF, relevantCharacters), kuf);
  lcd.createChar(setCharacter(RESH, relevantCharacters), resh);
  lcd.createChar(setCharacter(TET, relevantCharacters), tet);
  lcd.createChar(setCharacter(FEY, relevantCharacters), fey);
  lcd.createChar(setCharacter(ALEPH, relevantCharacters), aleph);
  lcd.createChar(setCharacter(LAMED, relevantCharacters), lamed);
  return lcd;
}

LiquidCrystal HebrewCharacterWriter::createLcdSpecialCharactersForSofShavuahTov(LiquidCrystal lcd, int relevantCharacters[]) {
  int newCharacters[] = {BET, VAV, TET, SHIN, SAMECH, FEYSOFIT, AYIN, 0};
  overwriteRelevantCharactersList(newCharacters, relevantCharacters);
  lcd.createChar(setCharacter(BET, relevantCharacters), bet);
  lcd.createChar(setCharacter(VAV, relevantCharacters), vav);
  lcd.createChar(setCharacter(TET, relevantCharacters), tet);
  lcd.createChar(setCharacter(SHIN, relevantCharacters), shin);
  lcd.createChar(setCharacter(SAMECH, relevantCharacters), samech);
  lcd.createChar(setCharacter(FEYSOFIT, relevantCharacters), feysofit);
  lcd.createChar(setCharacter(AYIN, relevantCharacters), ayin);
  return lcd;
}

LiquidCrystal HebrewCharacterWriter::createLcdSpecialCharactersForTimeUntilAlarm(LiquidCrystal lcd, int relevantCharacters[]) {
  int newCharacters[] = {VAV, KUF, TET, SHIN, AYIN, TAF, DALET, HEY};
  overwriteRelevantCharactersList(newCharacters, relevantCharacters);
  lcd.createChar(setCharacter(VAV, relevantCharacters), vav);
  lcd.createChar(setCharacter(KUF, relevantCharacters), kuf);
  lcd.createChar(setCharacter(TET, relevantCharacters), tet);
  lcd.createChar(setCharacter(SHIN, relevantCharacters), shin);
  lcd.createChar(setCharacter(AYIN, relevantCharacters), ayin);
  lcd.createChar(setCharacter(TAF, relevantCharacters), taf);
  lcd.createChar(setCharacter(DALET, relevantCharacters), dalet);
  lcd.createChar(setCharacter(HEY, relevantCharacters), hey);
  return lcd;
}

void HebrewCharacterWriter::writeHey(LiquidCrystal lcd, int startingCursor, int relevantCharacters[]) {
  lcd.setCursor(startingCursor, 0);
  lcd.write(getCharacter(HEY, relevantCharacters));
}

void HebrewCharacterWriter::writeShahot(LiquidCrystal lcd, int relevantCharacters[], int startingCursor, bool singular) {
  lcd.setCursor(startingCursor, 0);
  lcd.write(getCharacter(SHIN, relevantCharacters));
  lcd.setCursor(startingCursor - 1, 0);
  lcd.write(getCharacter(AYIN, relevantCharacters));
  if (singular) {
    writeHey(lcd, startingCursor - 2, relevantCharacters);
  } else {
    lcd.setCursor(startingCursor - 2, 0);
    lcd.write(getCharacter(VAV, relevantCharacters));
    lcd.setCursor(startingCursor - 3, 0);
    lcd.write(getCharacter(TAF, relevantCharacters));   
  }
}

void HebrewCharacterWriter::writeDakot(LiquidCrystal lcd, int relevantCharacters[], int startingCursor, bool singular) {
  lcd.setCursor(startingCursor, 0);
  lcd.write(getCharacter(DALET, relevantCharacters));
  lcd.setCursor(startingCursor - 1, 0);
  lcd.write(getCharacter(KUF, relevantCharacters));
  if (singular) {
    writeHey(lcd, startingCursor - 2, relevantCharacters);
  } else {
    lcd.setCursor(startingCursor - 2, 0);
    lcd.write(getCharacter(VAV, relevantCharacters));
    lcd.setCursor(startingCursor - 3, 0);
    lcd.write(getCharacter(TAF, relevantCharacters));    
  }
}
