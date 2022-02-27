#ifndef timecalculations_h
#define timecalculations_h
#include "Arduino.h"

struct HoursMinutesDuration;

class TimeCalculations {

public:
  TimeCalculations(long theTimeUntilWakeup, int theStartingDay);
  bool dayIsWeekendDay();
  bool isTimeToSoundAlarm(bool isHoliday);
  HoursMinutesDuration calculateTimeLeftUntilAlarm();
  bool isTimeLeftForPowerbank(int powerbankChargedIteration, long powerbankChargedCheckpoint);
  int getDayNumber();
  void setTime(long theTimeUntilWakeup);
  void setDay(int theStartingDay);
private:
  int calculateDayOfWeek();
  long timeUntilWakeup;
  int startingDay;

};
#endif
