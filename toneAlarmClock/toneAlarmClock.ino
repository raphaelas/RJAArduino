#include "weekdays.h"
#include "pitches.h"
#include "notedurations.h"
#include "hebrewcharacterwriter.h"
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

const int STOP_ALARM_SWITCH = A3;
const int POWERBANK_CHARGED_SWITCH = A2;
const int UPDATE_TIME_SWITCH = A1;
const int UPDATE_DAY_SWITCH = A0;
const int KEEP_POWERBANK_ALIVE_LED = 6;
const int TIME_IS_BEING_SET_OR_POWERBANK_CHARGED_LED = 7;
const int PIEZO_PIN = 8;
const int POWERBANK_IS_LOW_LED = 9;
const int RX_PIN = 10;
const int TX_PIN = 11;
const int DAY_IS_BEING_SET_LED = 12;
const int LCD_RS_PIN = 13;
const int LCD_E_PIN = 1;
const int LCD_D4_PIN = 5;
const int LCD_D5_PIN = 4;
const int LCD_D6_PIN = 3;
const int LCD_D7_PIN = 2;

SoftwareSerial softwareSerial(RX_PIN, TX_PIN);
LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

const int ONE_SECOND = 1000;
const int SECONDS_IN_MINUTE = 60, MINUTES_IN_HOUR = SECONDS_IN_MINUTE;
const long ONE_MINUTE = long(ONE_SECOND) * SECONDS_IN_MINUTE;
const long ONE_HOUR = ONE_MINUTE * MINUTES_IN_HOUR;
const long ONE_DAY = ONE_HOUR * 24;
const long THREE_DAYS = ONE_DAY * 3;

const int DELAY_BETWEEN_REPEATS = 500, DELAY_BETWEEN_SWITCH_LISTENS = DELAY_BETWEEN_REPEATS;
const int DELAY_DIVISOR = 10;

// This powerbank is the Arduino Powerbank 8000. In order to prevent the powerbank from
// shutting itself down after ~15 seconds of low power output, we blink a bright LED every
// 15 seconds.
const int KEEP_POWERBANK_ALIVE_COOLDOWN = ONE_SECOND * 15;
const int BRIEF_MOMENT = 50; int NOWISH = BRIEF_MOMENT;
const int LOW_SEVERITY_NOWISH = 5;

// These two variables should be set through an SD card over serial communication
// via the alarmClockTimeManager project however some starter values are needed.
const long STARTER_WAKEUP_TIME = ONE_MINUTE * 4;
const int STARTER_STARTING_DAY = TUESDAY;

long timeUntilWakeup;
int startingDay;

struct LcdScrollData lcdScrollData = {true, 0};
bool hasResetLcdMessagePosition = false;

const int ALARM_NOTE_COUNT = 8;
const int STARTUP_NOTE_COUNT = 4;

const int ALARM_NOTES[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

const int START_UP_NOTES[] = {
  NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4
};

const int ALARM_NOTE_DURATIONS[] = {
  QUARTER, EIGHTH, EIGHTH, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER
};

const int START_UP_NOTE_DURATIONS[] = {
  EIGHTH, EIGHTH, EIGHTH, EIGHTH
};

const float RECOMMENDED_NOTE_PAUSE_MULTIPLIER = 1.30;

int stopAlarmSwitchState = 0;
int updateTimeSwitchState = 0;
int updateDaySwitchState = 0;
int powerbankChargedSwitchState = 0;

bool keepSoundingAlarmClock = true;
bool hasWrittenBokerTov = false;
bool hasWrittenSofShavuahTov = false;
bool hasWrittenTimeUntilAlarmRecently = false;

const int MAX_COUNTDOWN = 3;
int countdownBlinkLightWhileAlarmSounding = 0;
unsigned long powerbankChargedCheckpoint = 0;

void setup() {
  pinMode(KEEP_POWERBANK_ALIVE_LED, OUTPUT);
  pinMode(TIME_IS_BEING_SET_OR_POWERBANK_CHARGED_LED, OUTPUT);
  pinMode(POWERBANK_IS_LOW_LED, OUTPUT);
  pinMode(DAY_IS_BEING_SET_LED, OUTPUT);
  pinMode(STOP_ALARM_SWITCH, INPUT);
  pinMode(POWERBANK_CHARGED_SWITCH, INPUT);
  pinMode(UPDATE_TIME_SWITCH, INPUT);
  pinMode(UPDATE_DAY_SWITCH, INPUT);
  softwareSerial.begin(38400);
  timeUntilWakeup = STARTER_WAKEUP_TIME;
  startingDay = STARTER_STARTING_DAY;
  playStartUpNotes();
}

void loop() {
  keepPowerbankOn();
  listenToSwitches();
  if (isTimeToSoundAlarm(startingDay) && keepSoundingAlarmClock) {
    handleTimeToSoundAlarm();
  } else if (isTimeToSoundAlarm(startingDay) && !keepSoundingAlarmClock) {
    if (!hasResetLcdMessagePosition) {
      lcdScrollData = resetLcdMessagePosition(lcd, lcdScrollData);
      hasResetLcdMessagePosition = true;
    }
  } else if (!isTimeToSoundAlarm(startingDay)) {
    handleNotTimeToSoundAlarm();
  }
}

void handleTimeToSoundAlarm() {
  if (!hasWrittenBokerTov) {
    writeBokerTov(lcd);
    hasWrittenBokerTov = true;
    hasResetLcdMessagePosition = false;
  }
  soundAlarm();
  lcdScrollData = scrollLcdMessage(lcd, lcdScrollData);
  keepPowerbankOnWhileAlarmSounding();
  splitDelayToCheckForSwitchPress(DELAY_BETWEEN_REPEATS);
}

void handleNotTimeToSoundAlarm() {
  keepSoundingAlarmClock = true;
  countdownBlinkLightWhileAlarmSounding = 0;
  hasWrittenBokerTov = false;
  if (!hasResetLcdMessagePosition) {
    lcdScrollData = resetLcdMessagePosition(lcd, lcdScrollData);
    hasResetLcdMessagePosition = true;
  }
  if (!hasWrittenTimeUntilAlarmRecently && !dayIsWeekendDay(startingDay)) {
    HoursMinutesDuration hoursMinutesDuration = calculateTimeLeftUntilAlarm(timeUntilWakeup);
    writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
    hasWrittenTimeUntilAlarmRecently = true;
  } else if (millis() % ONE_MINUTE < LOW_SEVERITY_NOWISH) {
    hasWrittenTimeUntilAlarmRecently = false;
  }
}

void soundAlarm() {
  for (int note = 0; note < ALARM_NOTE_COUNT; note++) {
    checkStopAlarmSwitchState();
    if (keepSoundingAlarmClock) {
      playNote(ALARM_NOTES[note], ALARM_NOTE_DURATIONS[note]);
    }
    if (note == (ALARM_NOTE_COUNT / 2)) {
      lcdScrollData = scrollLcdMessage(lcd, lcdScrollData);
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
    playNote(START_UP_NOTES[note], START_UP_NOTE_DURATIONS[note]);
  }
}

void blinkLight(int lightNumber) {
  digitalWrite(lightNumber, HIGH);
  delay(BRIEF_MOMENT);
  digitalWrite(lightNumber, LOW);
}

void keepPowerbankOnWhileAlarmSounding() {
  if (countdownBlinkLightWhileAlarmSounding == 0) {
    blinkLight(determineCorrectIndicatorLight());
    countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;
  } else {
    countdownBlinkLightWhileAlarmSounding--;
  }
}

void keepPowerbankOn() {
  unsigned long currentMillisWithinPowerbankKeepAliveCooldown = millis() % KEEP_POWERBANK_ALIVE_COOLDOWN;
  if (currentMillisWithinPowerbankKeepAliveCooldown < NOWISH) {
    blinkLight(determineCorrectIndicatorLight());
  }
}

int determineCorrectIndicatorLight() {
  long timeLeftForPowerbank = (powerbankChargedCheckpoint + THREE_DAYS) - millis();
  if (timeLeftForPowerbank > 0) {
    return KEEP_POWERBANK_ALIVE_LED;
  } else {
    return POWERBANK_IS_LOW_LED;
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

void checkPowerbankChargedSwitchState() {
  powerbankChargedSwitchState = digitalRead(POWERBANK_CHARGED_SWITCH);
  if (powerbankChargedSwitchState == HIGH) {
    powerbankChargedCheckpoint = millis();
    blinkLight(TIME_IS_BEING_SET_OR_POWERBANK_CHARGED_LED);
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void listenToUpdateTimeSwitch() {
  updateTimeSwitchState = digitalRead(UPDATE_TIME_SWITCH);
  if (updateTimeSwitchState == HIGH) {
    softwareSerial.write("timeplease");
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    long serialTimeIn = softwareSerial.parseInt();
    if (serialTimeIn > 0) {
      timeUntilWakeup = serialTimeIn;
      blinkLight(TIME_IS_BEING_SET_OR_POWERBANK_CHARGED_LED);
      keepSoundingAlarmClock = true;
      HoursMinutesDuration hoursMinutesDuration = calculateTimeLeftUntilAlarm(timeUntilWakeup);
      writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
    }
  }
}

void listenToUpdateDaySwitch() {
  updateDaySwitchState = digitalRead(UPDATE_DAY_SWITCH);
  if (updateDaySwitchState == HIGH) {
    softwareSerial.write("dayplease");
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    int serialDayIn = softwareSerial.parseInt();
    softwareSerial.write(serialDayIn);
    if (serialDayIn > 0) {
      startingDay = serialDayIn;
      blinkLight(DAY_IS_BEING_SET_LED);
      keepSoundingAlarmClock = true;
      HoursMinutesDuration hoursMinutesDuration = calculateTimeLeftUntilAlarm(timeUntilWakeup);
      writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
      dayIsWeekendDay(startingDay);
    }
  }
}

void listenToSwitches() {
  listenToUpdateTimeSwitch();
  listenToUpdateDaySwitch();
  checkPowerbankChargedSwitchState();
}

void splitDelayToCheckForSwitchPress(int delayAmount) {
  for (int i = 0; i < DELAY_DIVISOR; i++) {
    delay(delayAmount / DELAY_DIVISOR);
    checkStopAlarmSwitchState();
    listenToSwitches();
  }
}

int calculateDayOfWeek(int theStartingDay) {
  int mathUsableStartingDay = theStartingDay - 1;
  int startingDayMinusOne = (mathUsableStartingDay + (millis() / ONE_DAY)) % DAYS_IN_WEEK;
  return startingDayMinusOne + 1;
}

bool dayIsWeekendDay(int theStartingDay) {
  int currentDayOfWeek = calculateDayOfWeek(theStartingDay);
  for (int weekendDay = 0; weekendDay < (COUNT_WEEKEND_DAYS); weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == currentDayOfWeek) {
      if (!hasWrittenSofShavuahTov) {
        writeSofShavuahTov(lcd);
        hasWrittenSofShavuahTov = true;
      }
      return true;
    }
  }
  return false;
}

bool isTimeToSoundAlarm(int theStartingDay) {
  unsigned long currentMillisecondsWithinDay = millis() % ONE_DAY;
  long oneMinuteAfterWakeup = timeUntilWakeup + ONE_MINUTE;
  return currentMillisecondsWithinDay >= timeUntilWakeup && currentMillisecondsWithinDay < oneMinuteAfterWakeup
         && !dayIsWeekendDay(theStartingDay);
}

HoursMinutesDuration calculateTimeLeftUntilAlarm(long theTimeUntilWakeup) {
  long millisecondsUntilWakeup = theTimeUntilWakeup - (millis() % ONE_DAY);
  if (millisecondsUntilWakeup < 0) {
    millisecondsUntilWakeup += ONE_DAY;
  }
  int hoursLeftUntilAlarm = millisecondsUntilWakeup / ONE_HOUR;
  int minutesLeftUntilAlarm = (millisecondsUntilWakeup / ONE_MINUTE) % MINUTES_IN_HOUR;
  return (HoursMinutesDuration) {hoursLeftUntilAlarm, minutesLeftUntilAlarm};  
}
