long timeUntilWakeup;
int startingDay;

bool keepSoundingAlarmClock = true;
bool hasWrittenBokerTov = false;
bool hasWrittenSofShavuahTov = false;
bool hasWrittenChagSameach = false;
bool hasWrittenTimeUntilAlarmRecently = false;
bool isHoliday = false;
bool hasResetLcdMessagePosition = false;

int countdownBlinkLightWhileAlarmSounding = 0;
int powerbankChargedIteration = 0;
long powerbankChargedCheckpoint = 0;
