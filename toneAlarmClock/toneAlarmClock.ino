#include "tonealarmclockconstants.h"
#include "tonealarmclockglobalstatevariables.h"
#include "timeconstants.h"
#include "pinmappingconstants.h"
#include "tonealarmclockstructs.h"
#include <TimeCalculator.h>
#include <HebrewCharacterWriter.h>
#include <SwitchManager.h>
#include <LightManager.h>
#include <SerialManager.h>
#include <PiezoManager.h>
#include <Scheduler.h>   // This is mikaelpatel's Arduino-Scheduler for Uno compatibility

TimeCalculator timeCalculator(STARTER_WAKEUP_TIME, STARTER_STARTING_DAY);
HebrewCharacterWriter hebrewCharacterWriter(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
SerialManager serialManager(RX_PIN, TX_PIN);
SwitchManager switchManager(STOP_ALARM_OR_SET_HOLIDAY_SWITCH, POWERBANK_CHARGED_SWITCH, UPDATE_TIME_SWITCH, UPDATE_DAY_SWITCH);
LightManager lightManager(KEEP_POWERBANK_ALIVE_LED, POWERBANK_CHARGED_LED, POWERBANK_IS_LOW_OR_SERIAL_COMMUNICATION_FAILED_LED,
                          TIME_IS_BEING_SET_LED, DAY_IS_BEING_SET_LED);
PiezoManager piezoManager(PIEZO_PIN);

void setup() {
  Scheduler.startLoop(keepPowerbankOnAndListenToSwitchesLoop);
  piezoManager.playStartUpNotes();
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

void keepPowerbankOnAndListenToSwitchesLoop() {
  keepPowerbankOn();
  listenToSwitches();
  yield();
}

void handleTimeToSoundAlarm() {
  if (!hasWrittenBokerTov) {
    hebrewCharacterWriter.writeBokerTov();
    hasWrittenBokerTov = true;
    hasResetLcdMessagePosition = false;
  }
  int currentNote = 0;
  bool isAlarmTuneDone = false;
  while (!isAlarmTuneDone) {
    isAlarmTuneDone = soundAlarmAndScrollMessage(currentNote, keepSoundingAlarmClock);
    currentNote += 5;  
  }
  delay(DELAY_BETWEEN_REPEATS);
}

bool soundAlarmAndScrollMessage(int currentNote, bool keepSoundingAlarmClock) {
  bool isDone = piezoManager.soundAlarm(currentNote, currentNote + 5, keepSoundingAlarmClock);
  hebrewCharacterWriter.scrollLcdMessage();
  return isDone;
}

void handleNotTimeToSoundAlarm() {
  keepSoundingAlarmClock = true;
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
    updateTimeUntilAlarm();
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
        updateTimeUntilAlarm();
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
      long timeUntilWakeup = (serialTimeIn + millis()) % ONE_DAY;
      timeCalculator.setTime(timeUntilWakeup);
      lightManager.blinkLight(TIME_IS_BEING_SET_LED, BRIEF_MOMENT);
      keepSoundingAlarmClock = true;
      bool shouldWriteSofShavuahTov = timeCalculator.dayIsWeekendDay();
      if (shouldWriteSofShavuahTov) {
        hebrewCharacterWriter.writeSofShavuahTov();
        hasWrittenSofShavuahTov = true;
      } else {
        updateTimeUntilAlarm();
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
      int startingDay = serialDayIn;
      timeCalculator.setDay(startingDay);
      lightManager.blinkLight(DAY_IS_BEING_SET_LED, BRIEF_MOMENT);
      keepSoundingAlarmClock = true;
      bool shouldWriteSofShavuahTov = timeCalculator.dayIsWeekendDay();
      if (shouldWriteSofShavuahTov) {
        hebrewCharacterWriter.writeSofShavuahTov();
        hasWrittenSofShavuahTov = true;
      } else {
        updateTimeUntilAlarm();
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

void updateTimeUntilAlarm() {
  HoursMinutesDuration hoursMinutesDuration = timeCalculator.calculateTimeLeftUntilAlarm();
  hebrewCharacterWriter.writeTimeLeftUntilAlarmToLcd(hoursMinutesDuration);
  hasWrittenTimeUntilAlarmRecently = true;
}

void keepPowerbankOn() {
  int currentMillisWithinPowerbankKeepAliveCooldown = millis() % KEEP_POWERBANK_ALIVE_COOLDOWN;
  if (currentMillisWithinPowerbankKeepAliveCooldown < NOWISH) {
    bool isTimeLeftForPowerbank = timeCalculator.isTimeLeftForPowerbank(powerbankChargedIteration, powerbankChargedCheckpoint);
    lightManager.blinkLight(lightManager.getPowerbankLight(isTimeLeftForPowerbank), BRIEF_MOMENT);
  }
}

void listenToSwitches() {
  checkStopAlarmOrSetHolidaySwitchState();
  listenToUpdateTimeSwitch();
  listenToUpdateDaySwitch();
  checkPowerbankChargedSwitchState();
}
