#include "LightManager.h"
#include "Arduino.h"

LightManager::LightManager(int keepPowerbankAliveLed, int powerbankChargedLed, int powerbankIsLowOrSerialCommunicationFailedLed,
             int timeIsBeingSetLed, int dayIsBeingSetLed) {
  this->keepPowerbankAliveLed = keepPowerbankAliveLed;
  this->powerbankChargedLed = powerbankChargedLed;
  this->powerbankIsLowOrSerialCommunicationFailedLed = powerbankIsLowOrSerialCommunicationFailedLed;
  this->timeIsBeingSetLed = timeIsBeingSetLed;
  this->dayIsBeingSetLed = dayIsBeingSetLed;
  pinMode(keepPowerbankAliveLed, OUTPUT);
  pinMode(powerbankChargedLed, OUTPUT);
  pinMode(powerbankIsLowOrSerialCommunicationFailedLed, OUTPUT);
  pinMode(timeIsBeingSetLed, OUTPUT);
  pinMode(dayIsBeingSetLed, OUTPUT);
}

int LightManager::getPowerbankLight(bool isTimeLeftForPowerbank) {
  if (isTimeLeftForPowerbank) {
    return keepPowerbankAliveLed;
  } else {
    return powerbankIsLowOrSerialCommunicationFailedLed;
  }
}

void LightManager::blinkLight(int lightNumber, int lightOnTime) {
  digitalWrite(lightNumber, HIGH);
  delay(lightOnTime);
  digitalWrite(lightNumber, LOW);
}
