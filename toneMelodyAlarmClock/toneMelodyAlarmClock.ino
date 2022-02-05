#include "weekdays.h"
#include "pitches.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

const int COUNT_WEEKEND_DAYS = 2;
const int WEEKEND_DAYS[COUNT_WEEKEND_DAYS] = {SATURDAY, SUNDAY};
const int DAYS_IN_WEEK = 7;

const long ONE_MINUTE = 60000L;
const long ONE_MINUTE_AFTER_WAKEUP = timeUntilWakeup + ONE_MINUTE;
const long ONE_DAY = 86400000L;

const int DELAY_BETWEEN_REPEATS = 500, DELAY_BETWEEN_SWITCH_LISTENS = DELAY_BETWEEN_REPEATS;
const int DELAY_DIVISOR = 10;

const int KEEP_BATTERY_ALIVE_LED = 9;
const int STOP_ALARM_SWITCH = 2;
const int UPDATE_TIME_SWITCH = 3;

const int KEEP_PORTABLE_BATTERY_ALIVE_COOLDOWN = 14999;
const int BRIEF_MOMENT = 50, NOWISH = BRIEF_MOMENT;

long timeUntilWakeup = 180000;
int startingWeekday = MONDAY;

int stopAlarmSwitchState = 0;
int updateTimeSwitchState = 0;
bool keepSoundingAlarmClock = true;
int MAX_COUNTDOWN = 4, countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;

void setUpSerialMonitors() {
//  Serial.begin(9600);
  
//  mySerial.listen();
//  Serial.println(mySerial);
}

void setup() {
  pinMode(KEEP_BATTERY_ALIVE_LED, OUTPUT);
  pinMode(STOP_ALARM_SWITCH, INPUT);
  pinMode(UPDATE_TIME_SWITCH, INPUT);
  mySerial.begin(38400);
}

void blinkTheLight() {
    digitalWrite(KEEP_BATTERY_ALIVE_LED, HIGH);
    delay(BRIEF_MOMENT);
    digitalWrite(KEEP_BATTERY_ALIVE_LED, LOW);
}

void keepPortableArduinoBatteryOn() {
  unsigned long currentMillisWithinBatteryKeepAliveCooldown = millis() % KEEP_PORTABLE_BATTERY_ALIVE_COOLDOWN;
  if (currentMillisWithinBatteryKeepAliveCooldown < NOWISH) {
    blinkTheLight();
  }
}

void checkStopAlarmSwitchState() {
  stopAlarmSwitchState = digitalRead(STOP_ALARM_SWITCH);
  if (stopAlarmSwitchState == HIGH) {
    keepSoundingAlarmClock = false;
    blinkTheLight();
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
  }
}

void listenToUpdateTimeSwitch() {
  updateTimeSwitchState = digitalRead(UPDATE_TIME_SWITCH);
  if (updateTimeSwitchState == HIGH && mySerial.available()) {
    mySerial.write("go");
    delay(DELAY_BETWEEN_SWITCH_LISTENS);
    long serialTimeIn = mySerial.parseInt();
    if (serialTimeIn > 0) {
      timeUntilWakeup = serialTimeIn;
      keepSoundingAlarmClock = true;
    }    
  }
}

boolean dayIsWeekendDay(int day) {
  for (int weekendDay = 1; weekendDay < (COUNT_WEEKEND_DAYS + 1); weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == day) {
      return true;
    }
  }
  return false;
}

void splitDelayToCheckForStopPress(int delayAmount) {
  for (int i = 0; i < DELAY_DIVISOR; i++) {
    delay(delayAmount / DELAY_DIVISOR);
    checkStopAlarmSwitchState();
  }
}

bool isTimeToSoundAlarm(long currentMillisWithinDay, int currentDayOfWeek) {
  return currentMillisWithinDay >= timeUntilWakeup && currentMillisWithinDay < ONE_MINUTE_AFTER_WAKEUP
      && !dayIsWeekendDay(currentDayOfWeek);
}

void loop() {
  unsigned long currentMillisWithinDay = millis() % ONE_DAY;
  int currentDayOfWeek = STARTING_WEEKDAY + ((millis() / ONE_DAY) & DAYS_IN_WEEK);
  keepPortableArduinoBatteryOn();
  listenToUpdateTimeSwitch();
  if (isTimeToSoundAlarm(currentMillisWithinDay, currentDayOfWeek) && keepSoundingAlarmClock) {
    for (int thisNote = 0; thisNote < 8; thisNote++) {
      checkStopAlarmSwitchState();
      if (keepSoundingAlarmClock) {
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(8, melody[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        splitDelayToCheckForStopPress(pauseBetweenNotes);
        noTone(8);
      }
    }
    splitDelayToCheckForStopPress(DELAY_BETWEEN_REPEATS);
    if (countdownBlinkLightWhileAlarmSounding == 0) {
      blinkTheLight();
      countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;
    } else {
      countdownBlinkLightWhileAlarmSounding--;
    }
  } else if (!isTimeToSoundAlarm(currentMillisWithinDay, currentDayOfWeek)) {
    keepSoundingAlarmClock = true;
    countdownBlinkLightWhileAlarmSounding = MAX_COUNTDOWN;
  }
}
