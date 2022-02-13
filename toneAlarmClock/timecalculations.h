// This needs to be included after theconstants.h in toneAlarmClock because ONE_DAY,
// WEEKEND_DAYS, and ONE_MINUTE are already declared there and used in toneAlarmClock.
// This also needs to be included after hebrewcharacterwriter.h in toneAlarmClock
// because HoursMinutesDuration is already declared there and used in toneAlarmClock.

const int DAYS_IN_WEEK = 7;
const int COUNT_WEEKEND_DAYS = 2;

int calculateDayOfWeek(int theStartingDay) {
  int mathUsableStartingDay = theStartingDay - 1;
  int startingDayMinusOne = (mathUsableStartingDay + (millis() / ONE_DAY)) % DAYS_IN_WEEK;
  return startingDayMinusOne + 1;
}

bool dayIsWeekendDay(int theStartingDay) {
  int currentDayOfWeek = calculateDayOfWeek(theStartingDay);
  for (int weekendDay = 0; weekendDay < (COUNT_WEEKEND_DAYS); weekendDay++) {
    if (WEEKEND_DAYS[weekendDay] == currentDayOfWeek) {
      return true;
    }
  }
  return false;
}

bool isTimeToSoundAlarm(long theTimeUntilWakeup, int theStartingDay) {
  unsigned long currentMillisecondsWithinDay = millis() % ONE_DAY;
  long oneMinuteAfterWakeup = theTimeUntilWakeup + ONE_MINUTE;
  return currentMillisecondsWithinDay >= theTimeUntilWakeup && currentMillisecondsWithinDay < oneMinuteAfterWakeup
         && !dayIsWeekendDay(theStartingDay);
}


HoursMinutesDuration calculateTimeLeftUntilAlarm(long theTimeUntilWakeup) {
  long millisecondsUntilWakeup = theTimeUntilWakeup - (millis() % ONE_DAY);
  if (millisecondsUntilWakeup < 0) {
    millisecondsUntilWakeup += ONE_DAY;
  }
  int hoursLeftUntilAlarm = millisecondsUntilWakeup / ONE_HOUR;
  int minutesLeftUntilAlarm = (millisecondsUntilWakeup / ONE_MINUTE) % MINUTES_IN_HOUR;
  return (HoursMinutesDuration) {hoursLeftUntilAlarm, minutesLeftUntilAlarm};  
}
