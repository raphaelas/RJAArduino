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
#include <SerialManager.h>
#include <Scheduler.h>

TimeCalculator timeCalculator(STARTER_WAKEUP_TIME, STARTER_STARTING_DAY);
HebrewCharacterWriter hebrewCharacterWriter(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
SerialManager serialManager(RX_PIN, TX_PIN);
SwitchManager switchManager(STOP_ALARM_OR_SET_HOLIDAY_SWITCH, POWERBANK_CHARGED_SWITCH, UPDATE_TIME_SWITCH, UPDATE_DAY_SWITCH);
LightManager lightManager(KEEP_POWERBANK_ALIVE_LED, POWERBANK_CHARGED_LED, POWERBANK_IS_LOW_OR_SERIAL_COMMUNICATION_FAILED_LED,
                          TIME_IS_BEING_SET_LED, DAY_IS_BEING_SET_LED);

void setup() {
  Scheduler.startLoop(listenForStopButtonPressLoop);
  Scheduler.startLoop(keepPowerbankOnLoop);
  playStartUpNotes();
}

void loop() {
  if (timeCalculator.isTimeToSoundAlarm(isHoliday) && keepSoundingAlarmClock) {
    handleTimeToSoundAlarm();
  } else if (timeCalculator.isTimeToSoundAlarm(isHoliday) && !keepSoundingAlarmClock) {
    handleInBetweenStopButtonPressAndAlarmTimeEnding();
  } else if (!timeCalculator.isTimeToSoundAlarm(isHoliday)) {
    handleNotTimeToSoundAlarm();
  }
  yield();
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
  delay(DELAY_BETWEEN_REPEATS);
}

void listenForStopButtonPressLoop() {
  if (!keepSoundingAlarmClock || !timeCalculator.isTimeToSoundAlarm(isHoliday)) {
    yield();
  }
  checkStopAlarmOrSetHolidaySwitchState();
  yield();
}

void keepPowerbankOnLoop() {
  keepPowerbankOn();
  listenToSwitches();
  yield();
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
    if (keepSoundingAlarmClock) {
      playNote(ALARM_NOTES[note], ALARM_NOTE_DURATIONS[note], keepSoundingAlarmClock);
    }
  }
}

void playNote(int noteToPlay, int noteDuration, bool keepSoundingAlarmClock) {
  if (keepSoundingAlarmClock) {
    tone(PIEZO_PIN, noteToPlay, noteDuration);
    int pauseBetweenNotes = noteDuration * RECOMMENDED_NOTE_PAUSE_MULTIPLIER;
    delay(pauseBetweenNotes);
    noTone(PIEZO_PIN);
  }
}

void playStartUpNotes() {
  for (int note = 0; note < STARTUP_NOTE_COUNT; note++) {
    playNote(START_UP_NOTES[note], START_UP_NOTE_DURATIONS[note], keepSoundingAlarmClock);
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
  if (switchManager.isSwitchPressed(STOP_ALARM_OR_SET_HOLIDAY_SWITCH)) {
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
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void checkPowerbankChargedSwitchState() {
  if (switchManager.isSwitchPressed(POWERBANK_CHARGED_SWITCH)) {
    powerbankChargedIteration = millis() / POWERBANK_LIFE;
    powerbankChargedCheckpoint = millis() % POWERBANK_LIFE;
    lightManager.blinkLight(POWERBANK_CHARGED_LED, BRIEF_MOMENT);
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void listenToUpdateTimeSwitch() {
  if (switchManager.isSwitchPressed(UPDATE_TIME_SWITCH)) {
    bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
    serialManager.sendMessage(TIME_REQUEST);
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    long serialTimeIn = serialManager.parseMessage();
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
  if (switchManager.isSwitchPressed(UPDATE_DAY_SWITCH)) {
    bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
    serialManager.sendMessage(DAY_REQUEST);
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    int serialDayIn = serialManager.parseMessage();
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
  serialManager.restartSoftwareSerial();
  delay(ONE_SECOND * 3);
}

void listenToSwitches() {
  checkStopAlarmOrSetHolidaySwitchState();
  listenToUpdateTimeSwitch();
  listenToUpdateDaySwitch();
  checkPowerbankChargedSwitchState();
}
