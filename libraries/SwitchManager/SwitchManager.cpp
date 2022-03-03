#include "Arduino.h"
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

bool SwitchManager::isStopAlarmOrSetHolidaySwitchPressed() {
  return digitalRead(stopAlarmOrSetHolidaySwitch) == HIGH;
}

bool SwitchManager::isPowerbankChargedSwitchPressed() {
  return digitalRead(powerbankChargedSwitch) == HIGH;
}

bool SwitchManager::isUpdateTimeSwitchPressed() {
  return digitalRead(updateTimeSwitch) == HIGH;
}

bool SwitchManager::isUpdateDaySwitchPressed() {
  return digitalRead(updateDaySwitch) == HIGH;
}
