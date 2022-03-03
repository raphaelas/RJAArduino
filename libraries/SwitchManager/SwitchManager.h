#ifndef switchmanager_h
#define switchmanager_h
#include "Arduino.h"

class SwitchManager {
  public:
    SwitchManager(int stopAlarmOrSetHolidaySwitch, int powerbankChargedSwitch,
                  int updateTimeSwitch, int updateDaySwitch);
    bool isSwitchPressed(int switchNumber);
  private:
    int stopAlarmOrSetHolidaySwitch;
    int powerbankChargedSwitch;
    int updateTimeSwitch;
    int updateDaySwitch;

};


#endif
