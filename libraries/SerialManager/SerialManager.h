#ifndef serialmanager_h
#define serialmanager_h
#include "Arduino.h"
#include <SoftwareSerial.h>

class SerialManager {
  public:
    SerialManager(int rxPin, int txPin);
    void sendMessage(char * message);
    long parseMessage();
    void restartSoftwareSerial();
  private:
    SoftwareSerial * softwareSerial;
    int rxPin;
    int txPin;

};


#endif
