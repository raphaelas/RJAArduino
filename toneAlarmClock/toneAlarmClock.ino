#include "theconstants.h"
#include "tonealarmclockglobalstatevariables.h"
#include "timeconstants.h"
#include "musicalconstants.h"
#include "pinmappingconstants.h"
#include "tonealarmclockstructs.h"
#include <TimeCalculator.h>
#include <HebrewCharacterWriter.h>
#include <SwitchManager.h>
#include <LightManager.h>
#include <SoftwareSerial.h>

TimeCalculator timeCalculator(STARTER_WAKEUP_TIME, STARTER_STARTING_DAY);
HebrewCharacterWriter hebrewCharacterWriter(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
SwitchManager switchManager(STOP_ALARM_OR_SET_HOLIDAY_SWITCH, POWERBANK_CHARGED_SWITCH, UPDATE_TIME_SWITCH, UPDATE_DAY_SWITCH);
LightManager lightManager(KEEP_POWERBANK_ALIVE_LED, POWERBANK_CHARGED_LED, POWERBANK_IS_LOW_OR_SERIAL_COMMUNICATION_FAILED_LED,
                          TIME_IS_BEING_SET_LED, DAY_IS_BEING_SET_LED);

SoftwareSerial softwareSerial(RX_PIN, TX_PIN);

void setup() {
  softwareSerial.begin(38400);
  playStartUpNotes();
}

void loop() {
  keepPowerbankOn();
  listenToSwitches();
  if (timeCalculator.isTimeToSoundAlarm(isHoliday) && keepSoundingAlarmClock) {
    handleTimeToSoundAlarm();
  } else if (timeCalculator.isTimeToSoundAlarm(isHoliday) && !keepSoundingAlarmClock) {
    handleInBetweenStopButtonPressAndAlarmTimeEnding();
  } else if (!timeCalculator.isTimeToSoundAlarm(isHoliday)) {
    handleNotTimeToSoundAlarm();
  }
}

void handleTimeToSoundAlarm() {
  if (!hasWrittenBokerTov) {
    hebrewCharacterWriter.writeBokerTov();
    hasWrittenBokerTov = true;
    hasResetLcdMessagePosition = false;
  }
  soundAlarm(0, ALARM_NOTE_COUNT / 2);
  hebrewCharacterWriter.scrollLcdMessage();
  soundAlarm(ALARM_NOTE_COUNT / 2, ALARM_NOTE_COUNT);
  hebrewCharacterWriter.scrollLcdMessage();
  keepPowerbankOnWhileAlarmSounding();
  splitDelayToCheckForSwitchPress(DELAY_BETWEEN_REPEATS);
}

void handleNotTimeToSoundAlarm() {
  keepSoundingAlarmClock = true;
  countdownBlinkLightWhileAlarmSounding = 0;
  hasWrittenBokerTov = false;
  if (!hasResetLcdMessagePosition) {
    hebrewCharacterWriter.resetLcdMessagePosition();
    hasResetLcdMessagePosition = true;
    bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
  }
  if (timeCalculator.dayIsWeekendDay() && !hasWrittenSofShavuahTov) {
    hebrewCharacterWriter.writeSofShavuahTov();
    hasWrittenSofShavuahTov = true;
  } 
  else if (!hasWrittenTimeUntilAlarmRecently && !timeCalculator.dayIsWeekendDay() && !isHoliday) {
    HoursMinutesDuration hoursMinutesDuration = timeCalculator.calculateTimeLeftUntilAlarm();
    hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(hoursMinutesDuration);
    hasWrittenTimeUntilAlarmRecently = true;
  } else if (millis() % ONE_MINUTE < LOW_SEVERITY_NOWISH) {
    hasWrittenTimeUntilAlarmRecently = false;
  }
}

void handleInBetweenStopButtonPressAndAlarmTimeEnding() {
  if (!hasResetLcdMessagePosition) {
    hebrewCharacterWriter.resetLcdMessagePosition();
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

void keepPowerbankOnWhileAlarmSounding() {
  if (countdownBlinkLightWhileAlarmSounding == 0) {
    bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
    countdownBlinkLightWhileAlarmSounding = ALARM_SOUNDING_POWERBANK_MAX_COUNTDOWN;
  } else {
    countdownBlinkLightWhileAlarmSounding--;
  }
}

void keepPowerbankOn() {
  int currentMillisWithinPowerbankKeepAliveCooldown = millis() % KEEP_POWERBANK_ALIVE_COOLDOWN;
  if (currentMillisWithinPowerbankKeepAliveCooldown < NOWISH) {
    bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
  }
}

void checkStopAlarmOrSetHolidaySwitchState() {
  if (switchManager.isStopAlarmOrSetHolidaySwitchPressed()) {
    if (timeCalculator.isTimeToSoundAlarm(isHoliday)) {
      keepSoundingAlarmClock = false;
      bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
      lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
    } else {
      isHoliday = !isHoliday;
      if (isHoliday) {
        int dayNumber = timeCalculator.getDayNumber();
        hebrewCharacterWriter.writeChagSameach(dayNumber);
      } else {
        HoursMinutesDuration hoursMinutesDuration = timeCalculator.calculateTimeLeftUntilAlarm();
        hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(hoursMinutesDuration);
      }
    }
    splitDelayToKeepPowerbankOn(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void checkPowerbankChargedSwitchState() {
  if (switchManager.isPowerbankChargedSwitchPressed()) {
    powerbankChargedIteration = millis() / POWERBANK_LIFE;
    powerbankChargedCheckpoint = millis() % POWERBANK_LIFE;
    lightManager.blinkLight(POWERBANK_CHARGED_LED, BRIEF_MOMENT);
    splitDelayToKeepPowerbankOn(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void listenToUpdateTimeSwitch() {
  if (switchManager.isUpdateTimeSwitchPressed()) {
    bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
    softwareSerial.write(TIME_REQUEST);
    splitDelayToKeepPowerbankOn(DELAY_BETWEEN_SWITCH_LISTENS);
    long serialTimeIn = softwareSerial.parseInt();
    if (serialTimeIn > 0) {
      long theTimeUntilWakeup = (serialTimeIn + millis()) % ONE_DAY;
      timeCalculator.setTime(theTimeUntilWakeup);
      lightManager.blinkLight(TIME_IS_BEING_SET_LED, BRIEF_MOMENT);
      keepSoundingAlarmClock = true;
      bool shouldWriteSofShavuahTov = timeCalculator.dayIsWeekendDay();
      if (shouldWriteSofShavuahTov) {
        hebrewCharacterWriter.writeSofShavuahTov();
        hasWrittenSofShavuahTov = true;
      } else {
        HoursMinutesDuration hoursMinutesDuration = timeCalculator.calculateTimeLeftUntilAlarm();
        hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(hoursMinutesDuration);
      }
    } else {
      handleSerialCommunicationFailed();
    }
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
  }
}

void listenToUpdateDaySwitch() {
  if (switchManager.isUpdateDaySwitchPressed()) {
    bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
    softwareSerial.write(DAY_REQUEST);
    splitDelayToKeepPowerbankOn(DELAY_BETWEEN_SWITCH_LISTENS);
    int serialDayIn = softwareSerial.parseInt();
    if (serialDayIn > 0) {
      int theStartingDay = serialDayIn;
      timeCalculator.setDay(theStartingDay);
      lightManager.blinkLight(DAY_IS_BEING_SET_LED, BRIEF_MOMENT);
      keepSoundingAlarmClock = true;
      bool shouldWriteSofShavuahTov = timeCalculator.dayIsWeekendDay();
      if (shouldWriteSofShavuahTov) {
        hebrewCharacterWriter.writeSofShavuahTov();
        hasWrittenSofShavuahTov = true;
      } else {
        HoursMinutesDuration hoursMinutesDuration = timeCalculator.calculateTimeLeftUntilAlarm();
        hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(hoursMinutesDuration);
      }
    } else {
      handleSerialCommunicationFailed();
    }
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
  }
}

void handleSerialCommunicationFailed() {
  lightManager.blinkLight(POWERBANK_IS_LOW_OR_SERIAL_COMMUNICATION_FAILED_LED, BRIEF_MOMENT);
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
