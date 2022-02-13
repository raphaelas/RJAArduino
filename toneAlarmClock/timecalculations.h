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
