#ifndef timecalculations_h
#define timecalculations_h
#include "Arduino.h"

struct HoursMinutesDuration;

class TimeCalculations {

public:
  TimeCalculations();
  bool dayIsWeekendDay(int theStartingDay);
  bool isTimeToSoundAlarm(long theTimeUntilWakeup, int theStartingDay, bool isHoliday);
  HoursMinutesDuration calculateTimeLeftUntilAlarm(long theTimeUntilWakeup);
  bool isTimeLeftForPowerbank(int thePowerbankChargedIteration, long thePowerbankChargedCheckpoint);
  int getDayNumber();
private:
  int calculateDayOfWeek(int theStartingDay);

};
#endif
