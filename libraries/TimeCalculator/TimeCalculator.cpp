#include "TimeCalculator.h"
#include "Arduino.h"
#include "weekdays.h"
#include "timeconstants.h"
#include "tonealarmclockstructs.h"

TimeCalculator::TimeCalculator(long timeUntilWakeup, int startingDay) {
  this->timeUntilWakeup = timeUntilWakeup;
  this->startingDay = startingDay;
}

bool TimeCalculator::isTimeToSoundAlarm(bool isHoliday) {
  long currentMillisecondsWithinDay = millis() % ONE_DAY;
  long oneMinuteAfterWakeup = timeUntilWakeup + ONE_MINUTE;
  return currentMillisecondsWithinDay >= timeUntilWakeup && currentMillisecondsWithinDay < oneMinuteAfterWakeup
         && !dayIsWeekendDay() && !isHoliday;
}

bool TimeCalculator::dayIsWeekendDay() {
  int currentDayOfWeek = calculateDayOfWeek();
  for (int weekendDay = 0; weekendDay < (COUNT_WEEKEND_DAYS); weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == currentDayOfWeek) {
      return true;
    }
  }
  return false;
}

HoursMinutesDuration TimeCalculator::calculateTimeLeftUntilAlarm() {
  long millisecondsUntilWakeup = timeUntilWakeup - (millis() % ONE_DAY);
  if (millisecondsUntilWakeup < 0) {
    millisecondsUntilWakeup += ONE_DAY;
  }
  int hoursLeftUntilAlarm = millisecondsUntilWakeup / ONE_HOUR;
  int minutesLeftUntilAlarm = (millisecondsUntilWakeup / ONE_MINUTE) % MINUTES_IN_HOUR;
  return (HoursMinutesDuration) {hoursLeftUntilAlarm, minutesLeftUntilAlarm};
}

bool TimeCalculator::isTimeLeftForPowerbank(int powerbankChargedIteration, long powerbankChargedCheckpoint) {
  return (millis() / POWERBANK_LIFE) < powerbankChargedIteration + 1 ||
   (millis() / POWERBANK_LIFE == (powerbankChargedIteration + 1) && millis() % POWERBANK_LIFE < powerbankChargedCheckpoint);
}

int TimeCalculator::getDayNumber() {
  return ((millis() / ONE_DAY) % DAY_NUMBER_MAX_LENGTH) + 1;
}

void TimeCalculator::setTime(long timeUntilWakeup) {
  this->timeUntilWakeup = timeUntilWakeup;
}

void TimeCalculator::setDay(int startingDay) {
  this->startingDay = startingDay;
}

int TimeCalculator::calculateDayOfWeek() {
  int mathUsableStartingDay = startingDay - 1;
  int incrementDayTimeUntilWakeupHasPassed = (millis() % ONE_DAY) > timeUntilWakeup ? 1 : 0;
  int startingDayMinusOne = (mathUsableStartingDay + (millis() / ONE_DAY) + incrementDayTimeUntilWakeupHasPassed) % DAYS_IN_WEEK;
  return startingDayMinusOne + 1;
}
