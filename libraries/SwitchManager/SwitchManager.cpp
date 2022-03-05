#include "SwitchManager.h"

SwitchManager::SwitchManager(int stopAlarmOrSetHolidaySwitch, int powerbankChargedSwitch,
              int updateTimeSwitch, int updateDaySwitch) {
  this->stopAlarmOrSetHolidaySwitch = stopAlarmOrSetHolidaySwitch;
  this->powerbankChargedSwitch = powerbankChargedSwitch;
  this->updateTimeSwitch = updateTimeSwitch;
  this->updateDaySwitch = updateDaySwitch;
  pinMode(stopAlarmOrSetHolidaySwitch, INPUT);
  pinMode(powerbankChargedSwitch, INPUT);
  pinMode(updateTimeSwitch, INPUT);
  pinMode(updateDaySwitch, INPUT);
}

bool SwitchManager::isSwitchPressed(int switchNumber) {
  return digitalRead(switchNumber) == HIGH;
}
