#include "Arduino.h"
#include "TimeCalculations.h"
#include "weekdays.h"
#include "timeconstants.h"
#include "tonealarmclockstructs.h"

TimeCalculations::TimeCalculations() {}

bool TimeCalculations::isTimeToSoundAlarm(long theTimeUntilWakeup, int theStartingDay, bool isHoliday) {
  unsigned long currentMillisecondsWithinDay = millis() % ONE_DAY;
  long oneMinuteAfterWakeup = theTimeUntilWakeup + ONE_MINUTE;
  return currentMillisecondsWithinDay >= theTimeUntilWakeup && currentMillisecondsWithinDay < oneMinuteAfterWakeup
         && !dayIsWeekendDay(theStartingDay) && !isHoliday;
}

bool TimeCalculations::dayIsWeekendDay(int theStartingDay) {
  int currentDayOfWeek = calculateDayOfWeek(theStartingDay);
  for (int weekendDay = 0; weekendDay < (COUNT_WEEKEND_DAYS); weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == currentDayOfWeek) {
      return true;
    }
  }
  return false;
}

int TimeCalculations::calculateDayOfWeek(int theStartingDay) {
  int mathUsableStartingDay = theStartingDay - 1;
  int startingDayMinusOne = (mathUsableStartingDay + (millis() / ONE_DAY)) % DAYS_IN_WEEK;
  return startingDayMinusOne + 1;
}

HoursMinutesDuration TimeCalculations::calculateTimeLeftUntilAlarm(long theTimeUntilWakeup) {
  long millisecondsUntilWakeup = theTimeUntilWakeup - (millis() % ONE_DAY);
  if (millisecondsUntilWakeup < 0) {
    millisecondsUntilWakeup += ONE_DAY;
  }
  int hoursLeftUntilAlarm = millisecondsUntilWakeup / ONE_HOUR;
  int minutesLeftUntilAlarm = (millisecondsUntilWakeup / ONE_MINUTE) % MINUTES_IN_HOUR;
  return (HoursMinutesDuration) {hoursLeftUntilAlarm, minutesLeftUntilAlarm};
}

bool TimeCalculations::isTimeLeftForPowerbank(int thePowerbankChargedIteration, long thePowerbankChargedCheckpoint) {
  return millis() / POWERBANK_LIFE <= thePowerbankChargedIteration || millis() % POWERBANK_LIFE <= thePowerbankChargedCheckpoint;
}
