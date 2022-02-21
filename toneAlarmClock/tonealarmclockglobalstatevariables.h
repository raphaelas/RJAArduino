static long timeUntilWakeup;
static int startingDay;

bool hasResetLcdMessagePosition = false;

int stopAlarmSwitchState = 0;
int updateTimeSwitchState = 0;
int updateDaySwitchState = 0;
int powerbankChargedSwitchState = 0;

bool keepSoundingAlarmClock = true;
bool hasWrittenBokerTov = false;
bool hasWrittenSofShavuahTov = false;
bool hasWrittenTimeUntilAlarmRecently = false;

int countdownBlinkLightWhileAlarmSounding = 0;
unsigned long powerbankChargedCheckpoint = 0;
