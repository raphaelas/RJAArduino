#ifndef switchmanager_h
#define switchmanager_h
#include "Arduino.h"

class SwitchManager {
  public:
    SwitchManager(int stopAlarmOrSetHolidaySwitch, int powerbankChargedSwitch,
                  int updateTimeSwitch, int updateDaySwitch);
    bool isStopAlarmOrSetHolidaySwitchPressed();
    bool isPowerbankChargedSwitchPressed();
    bool isUpdateTimeSwitchPressed();
    bool isUpdateDaySwitchPressed();
  private:
    int stopAlarmOrSetHolidaySwitch;
    int powerbankChargedSwitch;
    int updateTimeSwitch;
    int updateDaySwitch;

};


#endif
