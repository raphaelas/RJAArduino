#include "theconstants.h"
#include "tonealarmclockglobalstatevariables.h"
#include "timeconstants.h"
#include "musicalconstants.h"
#include <HebrewCharacterWriter.h>
#include <TimeCalculations.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

const int TIME_IS_BEING_SET_LED = 19; // == A5
const int STOP_ALARM_OR_SET_HOLIDAY_SWITCH = 17; // == A3
const int POWERBANK_CHARGED_SWITCH = 16; // == A2
const int UPDATE_TIME_SWITCH = 15; // == A1
const int UPDATE_DAY_SWITCH = 14; // == A0
const int KEEP_POWERBANK_ALIVE_LED = 6;
const int POWERBANK_CHARGED_LED = 7;
const int PIEZO_PIN = 8;
const int POWERBANK_IS_LOW_OR_SERIAL_COMMUNICATION_FAILED_LED = 9;
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

HebrewCharacterWriter hebrewCharacterWriter = HebrewCharacterWriter();
TimeCalculations timeCalculations = TimeCalculations(STARTER_WAKEUP_TIME, STARTER_STARTING_DAY);

void setup() {
  pinMode(KEEP_POWERBANK_ALIVE_LED, OUTPUT);
  pinMode(POWERBANK_CHARGED_LED, OUTPUT);
  pinMode(POWERBANK_IS_LOW_OR_SERIAL_COMMUNICATION_FAILED_LED, OUTPUT);
  pinMode(TIME_IS_BEING_SET_LED, OUTPUT);
  pinMode(DAY_IS_BEING_SET_LED, OUTPUT);
  pinMode(STOP_ALARM_OR_SET_HOLIDAY_SWITCH, INPUT);
  pinMode(POWERBANK_CHARGED_SWITCH, INPUT);
  pinMode(UPDATE_TIME_SWITCH, INPUT);
  pinMode(UPDATE_DAY_SWITCH, INPUT);
  softwareSerial.begin(38400);
  playStartUpNotes();
}

void loop() {
  keepPowerbankOn();
  listenToSwitches();
  if (timeCalculations.isTimeToSoundAlarm(isHoliday) && keepSoundingAlarmClock) {
    handleTimeToSoundAlarm();
  } else if (timeCalculations.isTimeToSoundAlarm(isHoliday) && !keepSoundingAlarmClock) {
    handleInBetweenStopButtonPressAndAlarmTimeEnding();
  } else if (!timeCalculations.isTimeToSoundAlarm(isHoliday)) {
    handleNotTimeToSoundAlarm();
  }
}

void handleTimeToSoundAlarm() {
  if (!hasWrittenBokerTov) {
    hebrewCharacterWriter.writeBokerTov(lcd);
    hasWrittenBokerTov = true;
    hasResetLcdMessagePosition = false;
  }
  soundAlarm(0, ALARM_NOTE_COUNT / 2);
  lcdScrollData = hebrewCharacterWriter.scrollLcdMessage(lcd, lcdScrollData);
  soundAlarm(ALARM_NOTE_COUNT / 2, ALARM_NOTE_COUNT);
  lcdScrollData = hebrewCharacterWriter.scrollLcdMessage(lcd, lcdScrollData);
  keepPowerbankOnWhileAlarmSounding();
  splitDelayToCheckForSwitchPress(DELAY_BETWEEN_REPEATS);
}

void handleNotTimeToSoundAlarm() {
  keepSoundingAlarmClock = true;
  countdownBlinkLightWhileAlarmSounding = 0;
  hasWrittenBokerTov = false;
  if (!hasResetLcdMessagePosition) {
    lcdScrollData = hebrewCharacterWriter.resetLcdMessagePosition(lcd, lcdScrollData);
    hasResetLcdMessagePosition = true;
    bool isTimeLeftForPowerbank = timeCalculations.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    blinkLight(getPowerbankLight(isTimeLeftForPowerbank));
  }
  if (timeCalculations.dayIsWeekendDay() && !hasWrittenSofShavuahTov) {
    hebrewCharacterWriter.writeSofShavuahTov(lcd);
    hasWrittenSofShavuahTov = true;
  } else if (!hasWrittenTimeUntilAlarmRecently && !timeCalculations.dayIsWeekendDay() && !isHoliday) {
    HoursMinutesDuration hoursMinutesDuration = timeCalculations.calculateTimeLeftUntilAlarm();
    hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
    hasWrittenTimeUntilAlarmRecently = true;
  } else if (millis() % ONE_MINUTE < LOW_SEVERITY_NOWISH) {
    hasWrittenTimeUntilAlarmRecently = false;
  }
}

void handleInBetweenStopButtonPressAndAlarmTimeEnding() {
  if (!hasResetLcdMessagePosition) {
    lcdScrollData = hebrewCharacterWriter.resetLcdMessagePosition(lcd, lcdScrollData);
    hasResetLcdMessagePosition = true;
  }
}

void soundAlarm(int startingNote, int endingNote) {
  for (int note = startingNote; note < endingNote; note++) {
    checkStopAlarmOrSetHolidaySwitchState();
    if (keepSoundingAlarmClock) {
      playNote(ALARM_NOTES[note], ALARM_NOTE_DURATIONS[note]);
    }
  }
}

void playNote(int noteToPlay, int noteDuration) {
  tone(PIEZO_PIN, noteToPlay, noteDuration);
  int pauseBetweenNotes = noteDuration * RECOMMENDED_NOTE_PAUSE_MULTIPLIER;
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
    bool isTimeLeftForPowerbank = timeCalculations.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    blinkLight(getPowerbankLight(isTimeLeftForPowerbank));
    countdownBlinkLightWhileAlarmSounding = ALARM_SOUNDING_POWERBANK_MAX_COUNTDOWN;
  } else {
    countdownBlinkLightWhileAlarmSounding--;
  }
}

void keepPowerbankOn() {
  int currentMillisWithinPowerbankKeepAliveCooldown = millis() % KEEP_POWERBANK_ALIVE_COOLDOWN;
  if (currentMillisWithinPowerbankKeepAliveCooldown < NOWISH) {
    bool isTimeLeftForPowerbank = timeCalculations.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    blinkLight(getPowerbankLight(isTimeLeftForPowerbank));
  }
}

int getPowerbankLight(bool isTimeLeftForPowerbank) {
  if (isTimeLeftForPowerbank) {
    return KEEP_POWERBANK_ALIVE_LED;
  } else {
    return POWERBANK_IS_LOW_OR_SERIAL_COMMUNICATION_FAILED_LED;
  }
}

void checkStopAlarmOrSetHolidaySwitchState() {
  int stopAlarmOrSetHolidaySwitchState = digitalRead(STOP_ALARM_OR_SET_HOLIDAY_SWITCH);
  if (stopAlarmOrSetHolidaySwitchState == HIGH) {
    if (timeCalculations.isTimeToSoundAlarm(isHoliday)) {
      keepSoundingAlarmClock = false;
      bool isTimeLeftForPowerbank = timeCalculations.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
      blinkLight(getPowerbankLight(isTimeLeftForPowerbank));
    } else {
      isHoliday = !isHoliday;
      if (isHoliday) {
        int dayNumber = timeCalculations.getDayNumber();
        hebrewCharacterWriter.writeChagSameach(lcd, dayNumber);
      } else {
        HoursMinutesDuration hoursMinutesDuration = timeCalculations.calculateTimeLeftUntilAlarm();
        hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
      }
    }
    splitDelayToKeepPowerbankOn(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void checkPowerbankChargedSwitchState() {
  int powerbankChargedSwitchState = digitalRead(POWERBANK_CHARGED_SWITCH);
  if (powerbankChargedSwitchState == HIGH) {
    powerbankChargedIteration = millis() / POWERBANK_LIFE;
    powerbankChargedCheckpoint = millis() % POWERBANK_LIFE;
    blinkLight(POWERBANK_CHARGED_LED);
    splitDelayToKeepPowerbankOn(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void listenToUpdateTimeSwitch() {
  int updateTimeSwitchState = digitalRead(UPDATE_TIME_SWITCH);
  if (updateTimeSwitchState == HIGH) {
    bool isTimeLeftForPowerbank = timeCalculations.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    blinkLight(getPowerbankLight(isTimeLeftForPowerbank));
    softwareSerial.write(TIME_REQUEST);
    splitDelayToKeepPowerbankOn(DELAY_BETWEEN_SWITCH_LISTENS);
    long serialTimeIn = softwareSerial.parseInt();
    if (serialTimeIn > 0) {
      long timeUntilWakeup = (serialTimeIn + millis()) % ONE_DAY;
      timeCalculations.setDay(timeUntilWakeup);
      blinkLight(TIME_IS_BEING_SET_LED);
      keepSoundingAlarmClock = true;
      bool shouldWriteSofShavuahTov = timeCalculations.dayIsWeekendDay();
      if (shouldWriteSofShavuahTov) {
        hebrewCharacterWriter.writeSofShavuahTov(lcd);
        hasWrittenSofShavuahTov = true;
      } else {
        HoursMinutesDuration hoursMinutesDuration = timeCalculations.calculateTimeLeftUntilAlarm();
        hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
      }
    } else {
      handleSerialCommunicationFailed();
    }
    blinkLight(getPowerbankLight(isTimeLeftForPowerbank));
  }
}

void listenToUpdateDaySwitch() {
  int updateDaySwitchState = digitalRead(UPDATE_DAY_SWITCH);
  if (updateDaySwitchState == HIGH) {
    bool isTimeLeftForPowerbank = timeCalculations.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    blinkLight(getPowerbankLight(isTimeLeftForPowerbank));
    softwareSerial.write(DAY_REQUEST);
    splitDelayToKeepPowerbankOn(DELAY_BETWEEN_SWITCH_LISTENS);
    int serialDayIn = softwareSerial.parseInt();
    if (serialDayIn > 0) {
      int startingDay = serialDayIn;
      timeCalculations.setDay(startingDay);
      blinkLight(DAY_IS_BEING_SET_LED);
      keepSoundingAlarmClock = true;
      bool shouldWriteSofShavuahTov = timeCalculations.dayIsWeekendDay();
      if (shouldWriteSofShavuahTov) {
        hebrewCharacterWriter.writeSofShavuahTov(lcd);
        hasWrittenSofShavuahTov = true;
      } else {
        HoursMinutesDuration hoursMinutesDuration = timeCalculations.calculateTimeLeftUntilAlarm();
        hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
      }
    } else {
      handleSerialCommunicationFailed();
    }
    blinkLight(getPowerbankLight(isTimeLeftForPowerbank));
  }
}

void handleSerialCommunicationFailed() {
  blinkLight(POWERBANK_IS_LOW_OR_SERIAL_COMMUNICATION_FAILED_LED);
  softwareSerial = SoftwareSerial(RX_PIN, TX_PIN);
  softwareSerial.begin(38400);
  splitDelayToKeepPowerbankOn(ONE_SECOND * 3);
}

void splitDelayToKeepPowerbankOn(int delayAmount) {
  int delayIterations = delayAmount / BRIEF_MOMENT;
  for (int i = 0; i < delayIterations; i++) {
    keepPowerbankOn();
    delay(BRIEF_MOMENT);
  }
}

void listenToSwitches() {
  checkStopAlarmOrSetHolidaySwitchState();
  listenToUpdateTimeSwitch();
  listenToUpdateDaySwitch();
  checkPowerbankChargedSwitchState();
}

void splitDelayToCheckForSwitchPress(int delayAmount) {
  int delayIterations = delayAmount / BRIEF_MOMENT;
  for (int i = 0; i < delayIterations; i++) {
    listenToSwitches();
    delay(BRIEF_MOMENT);
  }
}
