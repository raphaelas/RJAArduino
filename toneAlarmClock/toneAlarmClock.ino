#include "weekdays.h"
#include "pitches.h"
#include "notedurations.h"
#include "hebrewcharacters.h"
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

const int ONE_SECOND = 1000;
const int SECONDS_IN_MINUTE = 60, MINUTES_IN_HOUR = SECONDS_IN_MINUTE;
const long ONE_MINUTE = long(ONE_SECOND) * SECONDS_IN_MINUTE;
const long ONE_HOUR = ONE_MINUTE * MINUTES_IN_HOUR;
const long ONE_DAY = ONE_HOUR * 24;
const long THREE_DAYS = ONE_DAY * 3;

const int COUNT_WEEKEND_DAYS = 2;
const int WEEKEND_DAYS[] = {SATURDAY, SUNDAY};
const int DAYS_IN_WEEK = 7;

const int DELAY_BETWEEN_REPEATS = 500, DELAY_BETWEEN_SWITCH_LISTENS = DELAY_BETWEEN_REPEATS;
const int DELAY_DIVISOR = 10;

const int STOP_ALARM_SWITCH = A3;
const int BATTERY_CHARGED_SWITCH = A2;
const int UPDATE_TIME_SWITCH = A1;
const int UPDATE_DAY_SWITCH = A0;
const int KEEP_BATTERY_ALIVE_LED = 6;
const int TIME_OR_DAY_OR_BATTERY_CHARGED_IS_BEING_SET_LED = 7;
const int PIEZO_PIN = 8;
const int BATTERY_IS_LOW_LED = 9;
const int RX_PIN = 10;
const int TX_PIN = 11;

SoftwareSerial softwareSerial(RX_PIN, TX_PIN);
LiquidCrystal lcd(13, 12, 5, 4, 3, 2);

const int KEEP_PORTABLE_BATTERY_ALIVE_COOLDOWN = 14500;
const int BRIEF_MOMENT = 50, NOWISH = BRIEF_MOMENT;

// These two variables should be set through an SD card over serial communication
// via the alarmClockTimeManager project however some starter values are needed.
const long STARTER_WAKEUP_TIME = 240000L;
const int STARTER_STARTING_DAY = TUESDAY;

long timeUntilWakeup;
long oneMinuteAfterWakeup;

int startingDay;
long serialTimeIn;
int serialDayIn;

long timeLeftForBattery;
unsigned long batteryChargedCheckpoint = 0;

const int ALARM_NOTE_COUNT = 8;
const int STARTUP_NOTE_COUNT = 4;

const int alarmNotes[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

const int startUpNotes[] = {
  NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4
};

const int alarmNoteDurations[] = {
  QUARTER, EIGHTH, EIGHTH, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER
};

const int startUpNoteDurations[] = {
  EIGHTH, EIGHTH, EIGHTH, EIGHTH
};

const float RECOMMENDED_NOTE_PAUSE_MULTIPLIER = 1.30;

int stopAlarmSwitchState = 0;
int updateTimeSwitchState = 0;
int updateDaySwitchState = 0;
int batteryChargedSwitchState = 0;

bool keepSoundingAlarmClock = true;
bool hasWrittenBokerTov = false;
bool hasWrittenSofShavuahTov = false;
bool hasWrittenTimeUntilAlarmRecently = false;
int MAX_COUNTDOWN = 1, countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;

const int DELAY_BETWEEN_TEXT_SCROLLS = 666;
const int MAX_SCROLL_AMOUNT = 3;

const int RELEVANT_CHARACTERS_COUNT = 8;
int relevantCharacters[RELEVANT_CHARACTERS_COUNT];
int recentRequest = 0;

byte bet[] = {
  B00000,
  B01110,
  B10001,
  B00001,
  B01101,
  B10010,
  B00000,
  B00000
};

byte vav[] = {
  B00000,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00000,
  B00000
};

byte kuf[] = {
  B00000,
  B01110,
  B00001,
  B00101,
  B00101,
  B00100,
  B00000,
  B00000
};

byte resh[] = {
  B00000,
  B01110,
  B00001,
  B00001,
  B00001,
  B00001,
  B00000,
  B00000
};

byte tet[] = {
  B00000,
  B01000,
  B10000,
  B10010,
  B01001,
  B00110,
  B00000,
  B00000
};

byte fey[] = {
  B00000,
  B11110,
  B00001,
  B01101,
  B10001,
  B01110,
  B00000,
  B00000
};

byte aleph[] = {
  B00000,
  B10011,
  B10100,
  B10100,
  B10100,
  B10011,
  B00000,
  B00000
};

byte lamed[] = {
  B00000,
  B00110,
  B00101,
  B11100,
  B10100,
  B01000,
  B00000,
  B00000
};

byte shin[] = {
  B00000,
  B01110,
  B10001,
  B10110,
  B10000,
  B01110,
  B00000,
  B00000
};

byte samech[] = {
  B00000,
  B00110,
  B01001,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000
};

byte feysofit[] = {
  B00000,
  B00111,
  B00101,
  B11111,
  B10100,
  B01000,
  B00000,
  B00000
};

byte ayin[] = {
  B00000,
  B01010,
  B00100,
  B01010,
  B01010,
  B00100,
  B00000,
  B00000
};

byte taf[] = {
  B00000,
  B01110,
  B01001,
  B01001,
  B01001,
  B11001,
  B00000,
  B00000
};

byte dalet[] = {
  B00000,
  B11100,
  B00010,
  B11001,
  B10101,
  B01110,
  B00100,
  B00000
};

byte hey[] = {
  B00000,
  B01100,
  B00010,
  B01001,
  B01001,
  B01001,
  B00000,
  B00000
};

void setup() {
  pinMode(KEEP_BATTERY_ALIVE_LED, OUTPUT);
  pinMode(TIME_OR_DAY_OR_BATTERY_CHARGED_IS_BEING_SET_LED, OUTPUT);
  pinMode(BATTERY_IS_LOW_LED, OUTPUT);
  pinMode(STOP_ALARM_SWITCH, INPUT);
  pinMode(BATTERY_CHARGED_SWITCH, INPUT);
  pinMode(UPDATE_TIME_SWITCH, INPUT);
  pinMode(UPDATE_DAY_SWITCH, INPUT);
  softwareSerial.begin(38400);
  setWakeupTimeVariables(STARTER_WAKEUP_TIME);
  startingDay = STARTER_STARTING_DAY;
  playStartUpNotes();
}

void loop() {
  keepPortableArduinoBatteryOn();
  listenToSwitches();
  unsigned long currentMillisWithinDay = millis() % ONE_DAY;
  int currentDayOfWeek = calculateDayOfWeek(startingDay);
  if (isTimeToSoundAlarm(currentMillisWithinDay, currentDayOfWeek) && keepSoundingAlarmClock) {
    for (int note = 0; note < ALARM_NOTE_COUNT; note++) {
      checkStopAlarmSwitchState();
      if (keepSoundingAlarmClock) {
        playNote(alarmNotes[note], alarmNoteDurations[note]);
      }
    }
    if (!hasWrittenBokerTov) {
      writeBokerTov();
    }
    scrollLcdMessage();
    keepPortableArduinoBatteryOnWhileAlarmSounding();      
  } else if (!isTimeToSoundAlarm(currentMillisWithinDay, currentDayOfWeek)) {
    keepSoundingAlarmClock = true;
    countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;
    if (!hasWrittenTimeUntilAlarmRecently && !dayIsWeekendDay(currentDayOfWeek)) {
      writeTimeLeftUntilAlarmToLcd();
    } else if (millis() % ONE_MINUTE < 2) {
      hasWrittenTimeUntilAlarmRecently = false;
    }
  }
}

void playNote(int noteToPlay, int noteDuration) {
  int noteDurationMilliseconds = ONE_SECOND / noteDuration;
  tone(PIEZO_PIN, noteToPlay, noteDurationMilliseconds);
  int pauseBetweenNotes = noteDurationMilliseconds * RECOMMENDED_NOTE_PAUSE_MULTIPLIER;
  splitDelayToCheckForSwitchPress(pauseBetweenNotes);
  noTone(PIEZO_PIN);
}

void playStartUpNotes() {
  for (int note = 0; note < STARTUP_NOTE_COUNT; note++) {
    playNote(startUpNotes[note], startUpNoteDurations[note]);
  }
}

void setWakeupTimeVariables(long theTimeUntilWakeup) {
  timeUntilWakeup = theTimeUntilWakeup;
  oneMinuteAfterWakeup = theTimeUntilWakeup + ONE_MINUTE;
}

void listenToSwitches() {
  listenToUpdateTimeSwitch();
  listenToUpdateDaySwitch();
  checkBatteryChargedSwitchState();
}

int calculateDayOfWeek(int theStartingDay) {
  int mathUsableStartingDay = theStartingDay - 1;
  int startingDayMinusOne = (mathUsableStartingDay + (millis() / ONE_DAY)) % DAYS_IN_WEEK;
  return startingDayMinusOne + 1;
}

void keepPortableArduinoBatteryOnWhileAlarmSounding() {
  if (countdownBlinkLightWhileAlarmSounding == 0) {
    blinkLight(determineCorrectIndicatorLight());
    countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;
  } else {
    countdownBlinkLightWhileAlarmSounding--;
  }
}

void blinkLight(int lightNumber) {
  digitalWrite(lightNumber, HIGH);
  delay(BRIEF_MOMENT);
  digitalWrite(lightNumber, LOW);
}

void keepPortableArduinoBatteryOn() {
  unsigned long currentMillisWithinBatteryKeepAliveCooldown = millis() % KEEP_PORTABLE_BATTERY_ALIVE_COOLDOWN;
  if (currentMillisWithinBatteryKeepAliveCooldown < NOWISH) {
    blinkLight(determineCorrectIndicatorLight());
  }
}

int determineCorrectIndicatorLight() {
  timeLeftForBattery = (batteryChargedCheckpoint + THREE_DAYS) - millis();
  if (timeLeftForBattery > 0) {
    return KEEP_BATTERY_ALIVE_LED;
  } else {
    return BATTERY_IS_LOW_LED;
  }
}

void checkStopAlarmSwitchState() {
  stopAlarmSwitchState = digitalRead(STOP_ALARM_SWITCH);
  if (stopAlarmSwitchState == HIGH) {
    keepSoundingAlarmClock = false;
    blinkLight(determineCorrectIndicatorLight());
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void checkBatteryChargedSwitchState() {
  batteryChargedSwitchState = digitalRead(BATTERY_CHARGED_SWITCH);
  if (batteryChargedSwitchState == HIGH) {
    batteryChargedCheckpoint = millis();
    blinkLight(TIME_OR_DAY_OR_BATTERY_CHARGED_IS_BEING_SET_LED);
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void listenToUpdateTimeSwitch() {
  updateTimeSwitchState = digitalRead(UPDATE_TIME_SWITCH);
  if (updateTimeSwitchState == HIGH) {
    recentRequest = 1;
    softwareSerial.write("timeplease");
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    if (recentRequest == 1) {
      serialTimeIn = softwareSerial.parseInt();
      if (serialTimeIn > 0) {
        setWakeupTimeVariables(serialTimeIn);
        blinkLight(TIME_OR_DAY_OR_BATTERY_CHARGED_IS_BEING_SET_LED);
        keepSoundingAlarmClock = true;
        writeTimeLeftUntilAlarmToLcd();
        recentRequest = 0;
      }
    }
  }
}

void listenToUpdateDaySwitch() {
  updateDaySwitchState = digitalRead(UPDATE_DAY_SWITCH);
  if (updateDaySwitchState == HIGH) {
    recentRequest = 2;
    softwareSerial.write("dayplease");
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    if (recentRequest == 2) {
      serialDayIn = softwareSerial.parseInt();
      softwareSerial.write(serialDayIn);
      if (serialDayIn > 0) {
        startingDay = serialDayIn;
        blinkLight(TIME_OR_DAY_OR_BATTERY_CHARGED_IS_BEING_SET_LED);
        keepSoundingAlarmClock = true;
        writeTimeLeftUntilAlarmToLcd();
        recentRequest = 0;
      }
    }
  }
}

boolean dayIsWeekendDay(int day) {
  for (int weekendDay = 0; weekendDay < (COUNT_WEEKEND_DAYS); weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == day) {
      if (!hasWrittenSofShavuahTov) {
        writeSofShavuahTov();
      }
      return true;
    }
  }
  return false;
}

void splitDelayToCheckForSwitchPress(int delayAmount) {
  for (int i = 0; i < DELAY_DIVISOR; i++) {
    delay(delayAmount / DELAY_DIVISOR);
    checkStopAlarmSwitchState();
    checkBatteryChargedSwitchState();
    listenToUpdateTimeSwitch();
    listenToUpdateDaySwitch();
  }
}

bool isTimeToSoundAlarm(long currentMillisWithinDay, int currentDayOfWeek) {
  return currentMillisWithinDay >= timeUntilWakeup && currentMillisWithinDay < oneMinuteAfterWakeup
         && !dayIsWeekendDay(currentDayOfWeek);
}

void createLcdSpecialCharactersForBokerTov() {
  int newCharacters[] = {BET, VAV, KUF, RESH, TET, FEY, ALEPH, LAMED};
  overwriteRelevantCharactersList(newCharacters);
  lcd.createChar(getCharacter(BET), bet);
  lcd.createChar(getCharacter(VAV), vav);
  lcd.createChar(getCharacter(KUF), kuf);
  lcd.createChar(getCharacter(RESH), resh);
  lcd.createChar(getCharacter(TET), tet);
  lcd.createChar(getCharacter(FEY), fey);
  lcd.createChar(getCharacter(ALEPH), aleph);
  lcd.createChar(getCharacter(LAMED), lamed);
}

void createLcdSpecialCharactersForSofShavuahTov() {
  int newCharacters[] = {BET, VAV, TET, SHIN, SAMECH, FEYSOFIT, AYIN, 0};
  overwriteRelevantCharactersList(newCharacters);
  lcd.createChar(getCharacter(BET), bet);
  lcd.createChar(getCharacter(VAV), vav);
  lcd.createChar(getCharacter(TET), tet);
  lcd.createChar(getCharacter(SHIN), shin);
  lcd.createChar(getCharacter(SAMECH), samech);
  lcd.createChar(getCharacter(FEYSOFIT), feysofit);
  lcd.createChar(getCharacter(AYIN), ayin);
}

void createLcdSpecialCharactersForTimeUntilAlarm() {
  int newCharacters[] = {VAV, KUF, TET, SHIN, AYIN, TAF, DALET, HEY};
  overwriteRelevantCharactersList(newCharacters);
  lcd.createChar(getCharacter(VAV), vav);
  lcd.createChar(getCharacter(KUF), kuf);
  lcd.createChar(getCharacter(TET), tet);
  lcd.createChar(getCharacter(SHIN), shin);
  lcd.createChar(getCharacter(AYIN), ayin);
  lcd.createChar(getCharacter(TAF), taf);
  lcd.createChar(getCharacter(DALET), dalet);
  lcd.createChar(getCharacter(HEY), hey);
}

void overwriteRelevantCharactersList(int newCharacters[]) {
  for (int i = 0; i < RELEVANT_CHARACTERS_COUNT; i++) {
    relevantCharacters[i] = newCharacters[i];
  }
}

int getCharacter(int characterToSearch) {
  for (int i = 0; i < RELEVANT_CHARACTERS_COUNT; i++) {
    if (characterToSearch == relevantCharacters[i]) {
      return i;
    }
  }
  return -1;
}

void writeTimeLeftUntilAlarmToLcd() {
  createLcdSpecialCharactersForTimeUntilAlarm();
  long millisecondsUntilWakeup = timeUntilWakeup - (millis() % ONE_DAY);
  if (millisecondsUntilWakeup < 0) {
    millisecondsUntilWakeup += ONE_DAY;
  }
  int hoursLeftUntilAlarm = millisecondsUntilWakeup / ONE_HOUR;
  int minutesLeftUntilAlarm = (millisecondsUntilWakeup / ONE_MINUTE) % MINUTES_IN_HOUR;
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(14, 0);
  int minutesCursor;
  if (hoursLeftUntilAlarm > 0) {
    lcd.print(hoursLeftUntilAlarm);
    bool singularHours = (hoursLeftUntilAlarm == 1);
    writeShahot(12, singularHours);
    lcd.setCursor(6 + singularHours, 0);
    lcd.print(minutesLeftUntilAlarm);
    writeDakot(4 + singularHours, minutesLeftUntilAlarm == 1);
  } else {
    lcd.print(minutesLeftUntilAlarm);
    writeDakot(12, minutesLeftUntilAlarm == 1);
  }
  hasWrittenTimeUntilAlarmRecently = true;
}

void writeShahot(int startingCursor, bool singular) {
  lcd.setCursor(startingCursor, 0);
  lcd.write(getCharacter(SHIN));
  lcd.setCursor(startingCursor - 1, 0);
  lcd.write(getCharacter(AYIN));
  if (singular) {
    writeHey(startingCursor - 2);
  } else {
    lcd.setCursor(startingCursor - 2, 0);
    lcd.write(getCharacter(VAV));
    lcd.setCursor(startingCursor - 3, 0);
    lcd.write(getCharacter(TAF));   
  }
}

void writeDakot(int startingCursor, bool singular) {
  lcd.setCursor(startingCursor, 0);
  lcd.write(getCharacter(DALET));
  lcd.setCursor(startingCursor - 1, 0);
  lcd.write(getCharacter(KUF));
  if (singular) {
    writeHey(startingCursor - 2);
  } else {
    lcd.setCursor(startingCursor - 2, 0);
    lcd.write(getCharacter(VAV));
    lcd.setCursor(startingCursor - 3, 0);
    lcd.write(getCharacter(TAF));    
  }
}

void writeHey(int startingCursor) {
  lcd.setCursor(startingCursor, 0);
  lcd.write(getCharacter(HEY));
}

void writeBokerTov() {
  createLcdSpecialCharactersForBokerTov();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(15, 0);
  lcd.write(getCharacter(BET));
  lcd.setCursor(14, 0);
  lcd.write(getCharacter(VAV));
  lcd.setCursor(13, 0);
  lcd.write(getCharacter(KUF));
  lcd.setCursor(12, 0);
  lcd.write(getCharacter(RESH));
  lcd.setCursor(10, 0);
  lcd.write(getCharacter(TET));
  lcd.setCursor(9, 0);
  lcd.write(getCharacter(VAV));
  lcd.setCursor(8, 0);
  lcd.write(getCharacter(BET));
  lcd.setCursor(6, 0);
  lcd.write(getCharacter(RESH));
  lcd.setCursor(5, 0);
  lcd.write(getCharacter(FEY));
  lcd.setCursor(4, 0);
  lcd.write(getCharacter(ALEPH));
  lcd.setCursor(3, 0);
  lcd.write(getCharacter(LAMED));
  hasWrittenBokerTov = true;
}

void writeSofShavuahTov() {
  createLcdSpecialCharactersForSofShavuahTov();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(15, 0);
  lcd.write(getCharacter(SAMECH));
  lcd.setCursor(14, 0);
  lcd.write(getCharacter(VAV));
  lcd.setCursor(13, 0);
  lcd.write(getCharacter(FEYSOFIT));
  lcd.setCursor(11, 0);
  lcd.write(getCharacter(SHIN));
  lcd.setCursor(10, 0);
  lcd.write(getCharacter(BET));
  lcd.setCursor(9, 0);
  lcd.write(getCharacter(VAV));
  lcd.setCursor(8, 0);
  lcd.write(getCharacter(AYIN));
  lcd.setCursor(6, 0);
  lcd.write(getCharacter(TET));
  lcd.setCursor(5, 0);
  lcd.write(getCharacter(VAV));
  lcd.setCursor(4, 0);
  lcd.write(getCharacter(BET));
  hasWrittenSofShavuahTov = true;
}

void scrollLcdMessage() {
  for (int positionCounter = 0; positionCounter < MAX_SCROLL_AMOUNT; positionCounter++) {
    lcd.scrollDisplayLeft();
    splitDelayToCheckForSwitchPress(DELAY_BETWEEN_TEXT_SCROLLS);
  }
  delay(DELAY_BETWEEN_TEXT_SCROLLS);
  for (int positionCounter = 0; positionCounter < MAX_SCROLL_AMOUNT; positionCounter++) {
    lcd.scrollDisplayRight();
    splitDelayToCheckForSwitchPress(DELAY_BETWEEN_TEXT_SCROLLS);
  }
}
