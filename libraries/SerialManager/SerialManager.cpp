#include "SerialManager.h"
#include "Arduino.h"

SerialManager::SerialManager(int rxPin, int txPin) {
  this->rxPin = rxPin;
  this->txPin = txPin;
  softwareSerial = new SoftwareSerial(rxPin, txPin);
  softwareSerial->begin(BAUD_RATE);
}

void SerialManager::sendMessage(char * message) {
  softwareSerial->write(message);
}

long SerialManager::parseMessage() {
  return softwareSerial->parseInt();
}

void SerialManager::restartSoftwareSerial() {
  delete(softwareSerial);
  softwareSerial = new SoftwareSerial(rxPin, txPin);
  softwareSerial->begin(BAUD_RATE);
}
