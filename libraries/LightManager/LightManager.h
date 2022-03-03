#ifndef lightmanager_h
#define lightmanager_h
#include "Arduino.h"

class LightManager {
  public:
    LightManager(int keepPowerbankAliveLed, int powerbankChargedLed, int powerbankIsLowOrSerialCommunicationFailedLed,
                 int timeIsBeingSetLed, int dayIsBeingSetLed);
    int getPowerbankLight(bool isTimeLeftForPowerbank);
    void blinkLight(int lightNumber, int lightOnTime);
  private:
    int keepPowerbankAliveLed;
    int powerbankChargedLed;
    int powerbankIsLowOrSerialCommunicationFailedLed;
    int timeIsBeingSetLed;
    int dayIsBeingSetLed;

};

#endif
