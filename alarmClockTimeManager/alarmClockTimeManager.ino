#include <SPI.h>
#include <SD.h>
#include "alarmClockTimeManagerGlobalVariables.h"
#include "sdcardconstants.h"
#include "alarmclocktimemanagertimeconstants.h"

const int TIME_INCREASE_SWITCH = 7;
const int TIME_DECREASE_SWITCH = 9;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TIME_INCREASE_SWITCH, INPUT);
  pinMode(TIME_DECREASE_SWITCH, INPUT);
  setUpSerialCommunicators();
}

void checkTimeIncreaseSwitchState() {
  int timeIncreaseSwitchState = digitalRead(TIME_INCREASE_SWITCH);
  if (timeIncreaseSwitchState == HIGH) {
    File timeFile = SD.open(alarmTimeFileName);
    long oldTime;
    while (timeFile.available()) {
      long parsedNumberBuffer = timeFile.parseInt();
      if (parsedNumberBuffer > 0) {
        oldTime = parsedNumberBuffer;
      }
    }
    timeFile.close();
    removeFile(alarmTimeFileName);
    long newTime = oldTime + (ONE_MINUTE * 10);
    File alarmTimeFile = SD.open(alarmTimeFileName, FILE_WRITE);
    if (alarmTimeFile) {
      alarmTimeFile.println(newTime);
    }
    alarmTimeFile.close();
    blinkOnboardLed();
  }
}

void checkTimeDecreaseSwitchState() {
  int timeDecreaseSwitchState = digitalRead(TIME_DECREASE_SWITCH);
  if (timeDecreaseSwitchState == HIGH) {
    File timeFile = SD.open(alarmTimeFileName);
    long oldTime;
    while (timeFile.available()) {
      long parsedNumberBuffer = timeFile.parseInt();
      if (parsedNumberBuffer > 0) {
        oldTime = parsedNumberBuffer;
      }
    }
    timeFile.close();
    removeFile(alarmTimeFileName);
    long newTime = oldTime - (ONE_MINUTE * 10);
    File alarmTimeFile = SD.open(alarmTimeFileName, FILE_WRITE);
    if (alarmTimeFile) {
      alarmTimeFile.println(newTime);
    }
    alarmTimeFile.close();
    blinkOnboardLed();
  }
}

void loop() {
  if (!alreadyInitializedSdCard) {
    initializeSdCard();
    alreadyInitializedSdCard = true;
  }
  if (Serial && !timeSet) {
    writeNewTimeToFile();
  } else if (Serial && !daySet) {
    writeNewDayToFile();
  }
  listenForTriggerFromOtherArduino();
  checkTimeIncreaseSwitchState();
  checkTimeDecreaseSwitchState();
  if (!alreadySentTimeToSerial1) {
    sendTimeToOtherArduino();
  } else if (!alreadySentDayToSerial1) {
    sendDayToOtherArduino();
  }
}

void printFileBeforeDeletingIt(String fileName) {
  File dataFile = SD.open(fileName);
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }
  else {
    Serial.println("error opening " + fileName);
  }
}

void removeFile(String fileName) {
  bool removalResult = SD.remove(fileName);
  Serial.println("Removal result: ");
  Serial.println(removalResult);    
  if (!SD.exists(fileName)) {
    Serial.println(fileName + " does not exist anymore.");
  }
}

void initializeSdCard() {
  if (!SD.begin(CHIP_SELECT_PIN)) {
    Serial.println("SD Card failed or not present");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(ONE_SECOND);
    digitalWrite(LED_BUILTIN, LOW);
    delay(ONE_SECOND);   
  } else if (Serial) {
    for (int file = 0; file < FILE_COUNT; file++) {
      String fileName = FILES_TO_REMOVE[file];
      if (SD.exists(fileName)) {
        Serial.println(fileName + " exists.");
        printFileBeforeDeletingIt(fileName);
        removeFile(fileName);
      } else {
        Serial.println(fileName + " does not exist.");
      }
    }
  }
}

void setUpSerialCommunicators() {
  Serial1.begin(38400);
  Serial.begin(9600);  // Interestingly, this line might not be needed.
  delay(SERIAL_MONITOR_STARTUP_DELAY);
}

void writeNewTimeToFile() {
  if (!promptedForTime && !SD.exists(alarmTimeFileName)) {
    Serial.println("python /home/$USER/Arduino/timeuntilalarm.py");
    Serial.println("Enter a time:");
    promptedForTime = true;
  }
  if (Serial.available()) {
    File alarmTimeFile = SD.open(alarmTimeFileName, FILE_WRITE);
    if (alarmTimeFile) {
      long newTime = Serial.parseInt();
      if (newTime > 0) {
        alarmTimeFile.println(newTime);
        Serial.println(newTime);
        timeSet = true;
      }
    }
    alarmTimeFile.close();
  }
}

void writeNewDayToFile() {
  if (!promptedForDay && !SD.exists(alarmDayFileName)) {
    Serial.println("Enter a day of week number from toneAlarmClock/weekdays.h. This");
    Serial.println("should be i.e. the day of the the next 8:00 AM occurrence.");
    Serial.println("timeuntilalarm.py prints out the correct day to enter.");
    promptedForDay = true;
  }
  if (Serial.available()) {
    File alarmDayFile = SD.open(alarmDayFileName, FILE_WRITE);
    if (alarmDayFile) {
      int newDay = Serial.parseInt();
      if (newDay > 0) {
        alarmDayFile.println(newDay);
        Serial.println(newDay);
        daySet = true;
        Serial.println("Arduino MKR is now ready to set Arduino Uno time variables.");
        Serial.println("Remember to connect the serial communication pins and a common ground.");
      }
    }
    alarmDayFile.close();
  }
}

void sendTimeToOtherArduino() {
  if (SD.exists(alarmTimeFileName)) {
    File alarmTimeFile = SD.open(alarmTimeFileName);
    if (alarmTimeFile) {
      byte nextByte;
      while (alarmTimeFile.available()) {
        nextByte = alarmTimeFile.read();
        if (isDigit(nextByte)) {
          Serial1.write(nextByte);
          Serial.write(nextByte);
        }
      }
      Serial.println();
      alreadySentTimeToSerial1 = true;
    }
    alarmTimeFile.close();
  }
}

void sendDayToOtherArduino() {
  if (SD.exists(alarmDayFileName)) {
    File alarmDayFile = SD.open(alarmDayFileName);
    if (alarmDayFile) {
      byte nextByte;
      while (alarmDayFile.available()) {
        nextByte = alarmDayFile.read();
        if (isDigit(nextByte)) {
          Serial1.write(nextByte);
          Serial.write(nextByte);
        }
      }
      Serial.println();
      alreadySentDayToSerial1 = true;
    }
    alarmDayFile.close();
  }
}

void listenForTriggerFromOtherArduino() {
  if (Serial1.available()) {
    String requestFromOtherArduino = Serial1.readString();
    blinkOnboardLed();
    Serial.println(requestFromOtherArduino);
    if (requestFromOtherArduino.equals("timeplease")) {
      alreadySentTimeToSerial1 = false;
    } else if (requestFromOtherArduino.equals("dayplease")) {
      alreadySentDayToSerial1 = false;
    }
  }
}

void blinkOnboardLed() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(BUILTIN_LED_ON_TIME);
  digitalWrite(LED_BUILTIN, LOW);
}
