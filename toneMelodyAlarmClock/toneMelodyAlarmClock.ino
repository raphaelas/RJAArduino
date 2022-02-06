#include "weekdays.h"
#include "pitches.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

const long ONE_MINUTE = 60000L;
const int ONE_SECOND = 1000;
const long ONE_DAY = 86400000L;
const long THREE_DAYS = 259200000L;

const int COUNT_WEEKEND_DAYS = 2;
const int WEEKEND_DAYS[COUNT_WEEKEND_DAYS] = {SATURDAY, SUNDAY};
const int DAYS_IN_WEEK = 7;

const int DELAY_BETWEEN_REPEATS = 500, DELAY_BETWEEN_SWITCH_LISTENS = DELAY_BETWEEN_REPEATS;
const int DELAY_DIVISOR = 10;

const int STOP_ALARM_SWITCH = 2;
const int UPDATE_TIME_SWITCH = 3;
const int UPDATE_DAY_SWITCH = 4;

const int KEEP_BATTERY_ALIVE_LED = 5;
const int TIME_OR_DAY_IS_BEING_SET_LED = 6;
const int BATTERY_IS_LOW_LED = 7;

const int KEEP_PORTABLE_BATTERY_ALIVE_COOLDOWN = 14000;
const int BRIEF_MOMENT = 50, NOWISH = BRIEF_MOMENT;

const long STARTER_WAKEUP_TIME = 180000L;
const int STARTER_STARTING_DAY = TUESDAY;

long timeUntilWakeup;
long oneMinuteAfterWakeup;

int startingDay;
int serialDayIn;

long timeLeftForBattery;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
const int NOTE_COUNT = 8;
const float RECOMMENDED_NOTE_PAUSE_MULTIPLIER = 1.30;

int stopAlarmSwitchState = 0;
int updateTimeSwitchState = 0;
int updateDaySwitchState = 0;
bool keepSoundingAlarmClock = true;
int MAX_COUNTDOWN = 4, countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;

void setup() {
  pinMode(KEEP_BATTERY_ALIVE_LED, OUTPUT);
  pinMode(TIME_OR_DAY_IS_BEING_SET_LED, OUTPUT);
  pinMode(BATTERY_IS_LOW_LED, OUTPUT);
  pinMode(STOP_ALARM_SWITCH, INPUT);
  pinMode(UPDATE_TIME_SWITCH, INPUT);
  pinMode(UPDATE_DAY_SWITCH, INPUT);
  mySerial.begin(38400);
// These next two variables should be set through an SD card over serial communication
// via the alarmClockTimeManager project however some starter values are needed.
  setWakeupTimeVariables(STARTER_WAKEUP_TIME);
  startingDay = STARTER_STARTING_DAY;
}

void loop() {
  unsigned long currentMillisWithinDay = millis() % ONE_DAY;
  int currentDayOfWeek = calculateDayOfWeek();
  keepPortableArduinoBatteryOn();
  listenToUpdateTimeSwitch();
  listenToUpdateDaySwitch();
  if (isTimeToSoundAlarm(currentMillisWithinDay, currentDayOfWeek)) {
    for (int note = 0; note < NOTE_COUNT; note++) {
      checkStopAlarmSwitchState();
      if (keepSoundingAlarmClock) {
        playMelodyNote(note);
      }
    }
    splitDelayToCheckForSwitchPress(DELAY_BETWEEN_REPEATS);
    keepPortableArduinoBatteryOnWhileAlarmSounding();
  } else {
    keepSoundingAlarmClock = true;
    countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;
  }
}

void playMelodyNote(int note) {
  int noteDuration = ONE_SECOND / noteDurations[note];
  tone(8, melody[note], noteDuration);
  int pauseBetweenNotes = noteDuration * RECOMMENDED_NOTE_PAUSE_MULTIPLIER;
  splitDelayToCheckForSwitchPress(pauseBetweenNotes);
  noTone(8);
}

void setWakeupTimeVariables(long theTimeUntilWakeup) {
  timeUntilWakeup = theTimeUntilWakeup;
  oneMinuteAfterWakeup = theTimeUntilWakeup + ONE_MINUTE;
}

int calculateDayOfWeek() {
  int mathUsableStartingDay = startingDay - 1;
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

void checkStopAlarmSwitchState() {
  stopAlarmSwitchState = digitalRead(STOP_ALARM_SWITCH);
  if (stopAlarmSwitchState == HIGH) {
    keepSoundingAlarmClock = false;
    blinkLight(determineCorrectIndicatorLight());
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

int determineCorrectIndicatorLight() {
  timeLeftForBattery = THREE_DAYS - millis();
  if (timeLeftForBattery > 0) {
    return KEEP_BATTERY_ALIVE_LED;
  } else {
    return BATTERY_IS_LOW_LED;
  }
}

void listenToUpdateTimeSwitch() {
  updateTimeSwitchState = digitalRead(UPDATE_TIME_SWITCH);
  if (updateTimeSwitchState == HIGH) {
    mySerial.write("timeplease");
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    long serialTimeIn = mySerial.parseInt();
    if (serialTimeIn > 0) {
      setWakeupTimeVariables(serialTimeIn);
      blinkLight(TIME_OR_DAY_IS_BEING_SET_LED);
      keepSoundingAlarmClock = true;
    }
  }
}

void listenToUpdateDaySwitch() {
  updateDaySwitchState = digitalRead(UPDATE_DAY_SWITCH);
  if (updateDaySwitchState == HIGH) {
    mySerial.write("dayplease");
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    serialDayIn = mySerial.parseInt();
    if (serialDayIn > 0) {
      startingDay = serialDayIn;
      blinkLight(TIME_OR_DAY_IS_BEING_SET_LED);
      keepSoundingAlarmClock = true;
    }
  }
}

boolean dayIsWeekendDay(int day) {
  for (int weekendDay = 0; weekendDay < (COUNT_WEEKEND_DAYS); weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == day) {
      return true;
    }
  }
  return false;
}

void splitDelayToCheckForSwitchPress(int delayAmount) {
  for (int i = 0; i < DELAY_DIVISOR; i++) {
    delay(delayAmount / DELAY_DIVISOR);
    checkStopAlarmSwitchState();
    listenToUpdateTimeSwitch();
    listenToUpdateDaySwitch();
  }
}

bool isTimeToSoundAlarm(long currentMillisWithinDay, int currentDayOfWeek) {
  return currentMillisWithinDay >= timeUntilWakeup && currentMillisWithinDay < oneMinuteAfterWakeup
         && !dayIsWeekendDay(currentDayOfWeek);
}
