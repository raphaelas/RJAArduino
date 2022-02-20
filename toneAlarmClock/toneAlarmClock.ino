#include "theconstants.h"
#include "tonealarmclockglobalstatevariables.h"
#include "timeconstants.h"
#include "musicalconstants.h"
#include <HebrewCharacterWriter.h>
#include <TimeCalculations.h>
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

HebrewCharacterWriter hebrewCharacterWriter;
TimeCalculations timeCalculations;

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
  if (timeCalculations.isTimeToSoundAlarm(timeUntilWakeup, startingDay) && keepSoundingAlarmClock) {
    handleTimeToSoundAlarm();
  } else if (timeCalculations.isTimeToSoundAlarm(timeUntilWakeup, startingDay) && !keepSoundingAlarmClock) {
    handleInBetweenStopButtonPressAndAlarmTimeEnding();
  } else if (!timeCalculations.isTimeToSoundAlarm(timeUntilWakeup, startingDay)) {
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
  }
  if (timeCalculations.dayIsWeekendDay(startingDay) && !hasWrittenSofShavuahTov) {
    hebrewCharacterWriter.writeSofShavuahTov(lcd);
    hasWrittenSofShavuahTov = true;
  } else if (!hasWrittenTimeUntilAlarmRecently && !timeCalculations.dayIsWeekendDay(startingDay)) {
    HoursMinutesDuration hoursMinutesDuration = timeCalculations.calculateTimeLeftUntilAlarm(timeUntilWakeup);
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
    checkStopAlarmSwitchState();
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
    blinkLight(determineCorrectIndicatorLight(powerbankChargedCheckpoint));
    countdownBlinkLightWhileAlarmSounding = ALARM_SOUNDING_POWERBANK_MAX_COUNTDOWN;
  } else {
    countdownBlinkLightWhileAlarmSounding--;
  }
}

void keepPowerbankOn() {
  unsigned long currentMillisWithinPowerbankKeepAliveCooldown = millis() % KEEP_POWERBANK_ALIVE_COOLDOWN;
  if (currentMillisWithinPowerbankKeepAliveCooldown < NOWISH) {
    blinkLight(determineCorrectIndicatorLight(powerbankChargedCheckpoint));
  }
}

int determineCorrectIndicatorLight(unsigned long thePowerbankChargedCheckpoint) {
  long timeLeftForPowerbank = (thePowerbankChargedCheckpoint + POWERBANK_LIFE) - millis();
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
    blinkLight(determineCorrectIndicatorLight(powerbankChargedCheckpoint));
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
      HoursMinutesDuration hoursMinutesDuration = timeCalculations.calculateTimeLeftUntilAlarm(timeUntilWakeup);
      hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
    }
  }
}

void listenToUpdateDaySwitch() {
  updateDaySwitchState = digitalRead(UPDATE_DAY_SWITCH);
  if (updateDaySwitchState == HIGH) {
    softwareSerial.write("dayplease");
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    int serialDayIn = softwareSerial.parseInt();
    if (serialDayIn > 0) {
      startingDay = serialDayIn;
      blinkLight(DAY_IS_BEING_SET_LED);
      keepSoundingAlarmClock = true;
      HoursMinutesDuration hoursMinutesDuration = timeCalculations.calculateTimeLeftUntilAlarm(timeUntilWakeup);
      hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(lcd, hoursMinutesDuration);
      boolean shouldWriteSofShavuahTov = timeCalculations.dayIsWeekendDay(startingDay);
      if (shouldWriteSofShavuahTov) {
        hebrewCharacterWriter.writeSofShavuahTov(lcd);
        hasWrittenSofShavuahTov = true;
      }
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
