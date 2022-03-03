#include "HebrewCharacterWriter.h"
#include "Arduino.h"
#include "hebrewcharacters.h"
#include "tonealarmclockstructs.h"

const int MAX_SCROLL_AMOUNT = 3;
const int RELEVANT_CHARACTERS_COUNT = 8;

HebrewCharacterWriter::HebrewCharacterWriter(int lcd_rs_pin, int lcd_e_pin, int lcd_d4_pin,
int lcd_d5_pin, int lcd_d6_pin, int lcd_d7_pin) {
  lcd = new LiquidCrystal(lcd_rs_pin, lcd_e_pin, lcd_d4_pin, lcd_d5_pin, lcd_d6_pin, lcd_d7_pin);
  lcdScrollData = new (LcdScrollData) {true, 0};
}

void HebrewCharacterWriter::writeTimeLeftUntilAlarmToLcd(HoursMinutesDuration hoursMinutesDuration) {
  int hoursLeftUntilAlarm = hoursMinutesDuration.hours;
  int minutesLeftUntilAlarm = hoursMinutesDuration.minutes;
  int relevantCharacters[RELEVANT_CHARACTERS_COUNT];
  createLcdSpecialCharactersForTimeUntilAlarm(relevantCharacters);
  lcd->begin(16, 2);
  lcd->clear();
  lcd->setCursor(14, 0);
  int minutesCursor;
  if (hoursLeftUntilAlarm > 0) {
    lcd->print(hoursLeftUntilAlarm);
    bool singularHours = (hoursLeftUntilAlarm == 1);
    writeShahot(relevantCharacters, 12, singularHours);
    lcd->setCursor(6 + singularHours, 0);
    lcd->print(minutesLeftUntilAlarm);
    writeDakot(relevantCharacters, 4 + singularHours, minutesLeftUntilAlarm == 1);
  } else {
    lcd->print(minutesLeftUntilAlarm);
    writeDakot(relevantCharacters, 12, minutesLeftUntilAlarm == 1);
  }
}

void HebrewCharacterWriter::writeBokerTov() {
  int relevantCharacters[RELEVANT_CHARACTERS_COUNT];
  createLcdSpecialCharactersForBokerTov(relevantCharacters);
  lcd->begin(16, 2);
  lcd->clear();
  lcd->setCursor(15, 0);
  lcd->write(getCharacter(BET, relevantCharacters));
  lcd->setCursor(14, 0);
  lcd->write(getCharacter(VAV, relevantCharacters));
  lcd->setCursor(13, 0);
  lcd->write(getCharacter(KUF, relevantCharacters));
  lcd->setCursor(12, 0);
  lcd->write(getCharacter(RESH, relevantCharacters));
  lcd->setCursor(10, 0);
  lcd->write(getCharacter(TET, relevantCharacters));
  lcd->setCursor(9, 0);
  lcd->write(getCharacter(VAV, relevantCharacters));
  lcd->setCursor(8, 0);
  lcd->write(getCharacter(BET, relevantCharacters));
  lcd->setCursor(6, 0);
  lcd->write(getCharacter(RESH, relevantCharacters));
  lcd->setCursor(5, 0);
  lcd->write(getCharacter(FEY, relevantCharacters));
  lcd->setCursor(4, 0);
  lcd->write(getCharacter(ALEPH, relevantCharacters));
  lcd->setCursor(3, 0);
  lcd->write(getCharacter(LAMED, relevantCharacters));
}

void HebrewCharacterWriter::writeSofShavuahTov() {
  int relevantCharacters[RELEVANT_CHARACTERS_COUNT];
  createLcdSpecialCharactersForSofShavuahTov(relevantCharacters);
  lcd->begin(16, 2);
  lcd->clear();
  lcd->setCursor(15, 0);
  lcd->write(getCharacter(SAMECH, relevantCharacters));
  lcd->setCursor(14, 0);
  lcd->write(getCharacter(VAV, relevantCharacters));
  lcd->setCursor(13, 0);
  lcd->write(getCharacter(FEYSOFIT, relevantCharacters));
  lcd->setCursor(11, 0);
  lcd->write(getCharacter(SHIN, relevantCharacters));
  lcd->setCursor(10, 0);
  lcd->write(getCharacter(BET, relevantCharacters));
  lcd->setCursor(9, 0);
  lcd->write(getCharacter(VAV, relevantCharacters));
  lcd->setCursor(8, 0);
  lcd->write(getCharacter(AYIN, relevantCharacters));
  lcd->setCursor(6, 0);
  lcd->write(getCharacter(TET, relevantCharacters));
  lcd->setCursor(5, 0);
  lcd->write(getCharacter(VAV, relevantCharacters));
  lcd->setCursor(4, 0);
  lcd->write(getCharacter(BET, relevantCharacters));
}

void HebrewCharacterWriter::writeChagSameach(int dayNumber) {
  int relevantCharacters[RELEVANT_CHARACTERS_COUNT];
  createLcdSpecialCharactersForChagSameach(relevantCharacters);
  lcd->begin(16, 2);
  lcd->clear();
  lcd->setCursor(15, 0);
  lcd->write(getCharacter(CHET, relevantCharacters));
  lcd->setCursor(14, 0);
  lcd->write(getCharacter(GIMEL, relevantCharacters));
  lcd->setCursor(12, 0);
  lcd->write(getCharacter(SHIN, relevantCharacters));
  lcd->setCursor(11, 0);
  lcd->write(getCharacter(MEM, relevantCharacters));
  lcd->setCursor(10, 0);
  lcd->write(getCharacter(CHAFSOFIT, relevantCharacters));
  writeDayNumber(relevantCharacters, dayNumber);
}

void HebrewCharacterWriter::writeDayNumber(int relevantCharacters[], int dayNumber) {
  lcd->setCursor(5, 1);
  lcd->write(getCharacter(YUD, relevantCharacters));
  lcd->setCursor(4, 1);
  lcd->write(getCharacter(VAV, relevantCharacters));
  lcd->setCursor(3, 1);
  lcd->write(getCharacter(MEMSOFIT, relevantCharacters));
  lcd->setCursor(0, 1);
  lcd->print(dayNumber);
}

void HebrewCharacterWriter::scrollLcdMessage() {
  bool scrollLeft = lcdScrollData->scrollLeft;
  int scrollPositionCounter = lcdScrollData->scrollPositionCounter;
  if (scrollPositionCounter == MAX_SCROLL_AMOUNT) {
    scrollLeft = !scrollLeft;
    scrollPositionCounter = 0;
  }
  if (scrollLeft) {
    lcd->scrollDisplayLeft();
  } else {
    lcd->scrollDisplayRight();
  }
  scrollPositionCounter++;
  delete(lcdScrollData);
  lcdScrollData = new (LcdScrollData) {scrollLeft, scrollPositionCounter};
}

void HebrewCharacterWriter::resetLcdMessagePosition() {
  bool scrollLeft = lcdScrollData->scrollLeft;
  int scrollPositionCounter = lcdScrollData->scrollPositionCounter;
  if (!scrollLeft) {
    scrollPositionCounter = MAX_SCROLL_AMOUNT - scrollPositionCounter;
  }
  while (scrollPositionCounter > 0) {
    lcd->scrollDisplayRight();
    scrollPositionCounter--;
  }
  scrollLeft = true;
  delete(lcdScrollData);
  lcdScrollData = new (LcdScrollData) {scrollLeft, scrollPositionCounter};
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

void HebrewCharacterWriter::createLcdSpecialCharactersForBokerTov(int relevantCharacters[]) {
  int newCharacters[] = {BET, VAV, KUF, RESH, TET, FEY, ALEPH, LAMED};
  overwriteRelevantCharactersList(newCharacters, relevantCharacters);
  lcd->createChar(getCharacter(BET, relevantCharacters), bet);
  lcd->createChar(getCharacter(VAV, relevantCharacters), vav);
  lcd->createChar(getCharacter(KUF, relevantCharacters), kuf);
  lcd->createChar(getCharacter(RESH, relevantCharacters), resh);
  lcd->createChar(getCharacter(TET, relevantCharacters), tet);
  lcd->createChar(getCharacter(FEY, relevantCharacters), fey);
  lcd->createChar(getCharacter(ALEPH, relevantCharacters), aleph);
  lcd->createChar(getCharacter(LAMED, relevantCharacters), lamed);
}

void HebrewCharacterWriter::createLcdSpecialCharactersForSofShavuahTov(int relevantCharacters[]) {
  int newCharacters[] = {BET, VAV, TET, SHIN, SAMECH, FEYSOFIT, AYIN, 0};
  overwriteRelevantCharactersList(newCharacters, relevantCharacters);
  lcd->createChar(getCharacter(BET, relevantCharacters), bet);
  lcd->createChar(getCharacter(VAV, relevantCharacters), vav);
  lcd->createChar(getCharacter(TET, relevantCharacters), tet);
  lcd->createChar(getCharacter(SHIN, relevantCharacters), shin);
  lcd->createChar(getCharacter(SAMECH, relevantCharacters), samech);
  lcd->createChar(getCharacter(FEYSOFIT, relevantCharacters), feysofit);
  lcd->createChar(getCharacter(AYIN, relevantCharacters), ayin);
}

void HebrewCharacterWriter::createLcdSpecialCharactersForChagSameach(int relevantCharacters[]) {
  int newCharacters[] = {CHET, GIMEL, SHIN, MEM, CHAFSOFIT, YUD, VAV, MEMSOFIT};
  overwriteRelevantCharactersList(newCharacters, relevantCharacters);
  lcd->createChar(getCharacter(CHET, relevantCharacters), chet);
  lcd->createChar(getCharacter(GIMEL, relevantCharacters), gimel);
  lcd->createChar(getCharacter(SHIN, relevantCharacters), shin);
  lcd->createChar(getCharacter(MEM, relevantCharacters), mem);
  lcd->createChar(getCharacter(CHAFSOFIT, relevantCharacters), chafsofit);
  lcd->createChar(getCharacter(YUD, relevantCharacters), yud);
  lcd->createChar(getCharacter(VAV, relevantCharacters), vav);
  lcd->createChar(getCharacter(MEMSOFIT, relevantCharacters), memsofit);
}

void HebrewCharacterWriter::createLcdSpecialCharactersForTimeUntilAlarm(int relevantCharacters[]) {
  int newCharacters[] = {VAV, KUF, TET, SHIN, AYIN, TAF, DALET, HEY};
  overwriteRelevantCharactersList(newCharacters, relevantCharacters);
  lcd->createChar(getCharacter(VAV, relevantCharacters), vav);
  lcd->createChar(getCharacter(KUF, relevantCharacters), kuf);
  lcd->createChar(getCharacter(TET, relevantCharacters), tet);
  lcd->createChar(getCharacter(SHIN, relevantCharacters), shin);
  lcd->createChar(getCharacter(AYIN, relevantCharacters), ayin);
  lcd->createChar(getCharacter(TAF, relevantCharacters), taf);
  lcd->createChar(getCharacter(DALET, relevantCharacters), dalet);
  lcd->createChar(getCharacter(HEY, relevantCharacters), hey);
}

void HebrewCharacterWriter::writeHey(int startingCursor, int relevantCharacters[]) {
  lcd->setCursor(startingCursor, 0);
  lcd->write(getCharacter(HEY, relevantCharacters));
}

void HebrewCharacterWriter::writeShahot(int relevantCharacters[], int startingCursor, bool singular) {
  lcd->setCursor(startingCursor, 0);
  lcd->write(getCharacter(SHIN, relevantCharacters));
  lcd->setCursor(startingCursor - 1, 0);
  lcd->write(getCharacter(AYIN, relevantCharacters));
  if (singular) {
    writeHey(startingCursor - 2, relevantCharacters);
  } else {
    lcd->setCursor(startingCursor - 2, 0);
    lcd->write(getCharacter(VAV, relevantCharacters));
    lcd->setCursor(startingCursor - 3, 0);
    lcd->write(getCharacter(TAF, relevantCharacters));
  }
}

void HebrewCharacterWriter::writeDakot(int relevantCharacters[], int startingCursor, bool singular) {
  lcd->setCursor(startingCursor, 0);
  lcd->write(getCharacter(DALET, relevantCharacters));
  lcd->setCursor(startingCursor - 1, 0);
  lcd->write(getCharacter(KUF, relevantCharacters));
  if (singular) {
    writeHey(startingCursor - 2, relevantCharacters);
  } else {
    lcd->setCursor(startingCursor - 2, 0);
    lcd->write(getCharacter(VAV, relevantCharacters));
    lcd->setCursor(startingCursor - 3, 0);
    lcd->write(getCharacter(TAF, relevantCharacters));
  }
}
