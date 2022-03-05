#ifndef timecalculator_h
#define timecalculator_h
#include "Arduino.h"

struct HoursMinutesDuration;

class TimeCalculator {

  public:
    TimeCalculator(long timeUntilWakeup, int startingDay);
    bool dayIsWeekendDay();
    bool isTimeToSoundAlarm(bool isHoliday);
    HoursMinutesDuration calculateTimeLeftUntilAlarm();
    bool isTimeLeftForPowerbank(int powerbankChargedIteration, long powerbankChargedCheckpoint);
    int getDayNumber();
    void setTime(long timeUntilWakeup);
    void setDay(int startingDay);
    long timeUntilWakeup;
    int startingDay;
  private:
    int calculateDayOfWeek();

};
#endif
