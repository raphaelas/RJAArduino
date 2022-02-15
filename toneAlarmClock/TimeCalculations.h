#ifndef timecalculations_h
#define timecalculations_h
#include "Arduino.h"
#include "theconstants.h"
#include "HebrewCharacterWriter.h"

class TimeCalculations {
  
public:
  TimeCalculations();
  bool dayIsWeekendDay(int theStartingDay);
  bool isTimeToSoundAlarm(long theTimeUntilWakeup, int theStartingDay);
  HoursMinutesDuration calculateTimeLeftUntilAlarm(long theTimeUntilWakeup);
private:
  int calculateDayOfWeek(int theStartingDay);

};
#endif
