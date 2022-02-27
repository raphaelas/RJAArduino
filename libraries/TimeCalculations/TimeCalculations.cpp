#include "Arduino.h"
#include "TimeCalculations.h"
#include "weekdays.h"
#include "timeconstants.h"
#include "tonealarmclockstructs.h"

TimeCalculations::TimeCalculations(long theTimeUntilWakeup, int theStartingDay) {
  timeUntilWakeup = theTimeUntilWakeup;
  startingDay = theStartingDay;
}

bool TimeCalculations::isTimeToSoundAlarm(bool isHoliday) {
  long currentMillisecondsWithinDay = millis() % ONE_DAY;
  long oneMinuteAfterWakeup = timeUntilWakeup + ONE_MINUTE;
  return currentMillisecondsWithinDay >= timeUntilWakeup && currentMillisecondsWithinDay < oneMinuteAfterWakeup
         && !dayIsWeekendDay() && !isHoliday;
}

bool TimeCalculations::dayIsWeekendDay() {
  int currentDayOfWeek = calculateDayOfWeek();
  for (int weekendDay = 0; weekendDay < (COUNT_WEEKEND_DAYS); weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == currentDayOfWeek) {
      return true;
    }
  }
  return false;
}

HoursMinutesDuration TimeCalculations::calculateTimeLeftUntilAlarm() {
  long millisecondsUntilWakeup = timeUntilWakeup - (millis() % ONE_DAY);
  if (millisecondsUntilWakeup < 0) {
    millisecondsUntilWakeup += ONE_DAY;
  }
  int hoursLeftUntilAlarm = millisecondsUntilWakeup / ONE_HOUR;
  int minutesLeftUntilAlarm = (millisecondsUntilWakeup / ONE_MINUTE) % MINUTES_IN_HOUR;
  return (HoursMinutesDuration) {hoursLeftUntilAlarm, minutesLeftUntilAlarm};
}

bool TimeCalculations::isTimeLeftForPowerbank(int powerbankChargedIteration, long powerbankChargedCheckpoint) {
  return (millis() / POWERBANK_LIFE) < powerbankChargedIteration + 1 ||
   (millis() / POWERBANK_LIFE == (powerbankChargedIteration + 1) && millis() % POWERBANK_LIFE < powerbankChargedCheckpoint);
}

int TimeCalculations::getDayNumber() {
  return ((millis() / ONE_DAY) % DAY_NUMBER_MAX_LENGTH) + 1;
}

void TimeCalculations::setTime(long theTimeUntilWakeup) {
  timeUntilWakeup = theTimeUntilWakeup;
}

void TimeCalculations::setDay(int theStartingDay) {
  startingDay = theStartingDay;
}

int TimeCalculations::calculateDayOfWeek() {
  int mathUsableStartingDay = startingDay - 1;
  int incrementDayTimeUntilWakeupHasPassed = (millis() % ONE_DAY) > timeUntilWakeup ? 1 : 0;
  int startingDayMinusOne = (mathUsableStartingDay + (millis() / ONE_DAY) + incrementDayTimeUntilWakeupHasPassed) % DAYS_IN_WEEK;
  return startingDayMinusOne + 1;
}
