#include <SPI.h>
#include <SD.h>

const int CHIP_SELECT = 4;
String alarmTimeFileName = "alarmset.txt";
String alarmDayFileName = "alarmday.txt";
File alarmTimeFile;
File alarmDayFile;
int MAX_FILE_SIZE = 12;

bool alreadySentTimeToSerial1 = true;
bool alreadySentDayToSerial1 = true;
bool timeSet = false;
bool daySet = false;

bool promptedForTime = false;
bool promptedForDay = false;

const int SERIAL_MONITOR_STARTUP_DELAY = 3000;

void setup() {
  setUpSerialMonitors();
  initializeSdCard();
}

void loop() {
  if (!timeSet) {
    writeNewTimeToFile();
  } else if (!daySet) {
    writeNewDayToFile();
  }
  listenForTriggerFromOtherArduino();
  if (!alreadySentTimeToSerial1) {
    sendTimeToOtherArduino();
  } else if (!alreadySentDayToSerial1) {
    sendDayToOtherArduino();
  }
}

void initializeSdCard() {
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
}

void setUpSerialMonitors() {
  Serial1.begin(38400);
  Serial.begin(9600);
  delay(SERIAL_MONITOR_STARTUP_DELAY);
}

void writeNewTimeToFile() {
  if (!promptedForTime && !SD.exists(alarmTimeFileName)) {
    Serial.println("Enter a time:");
    promptedForTime = true;
  }
  if (Serial.available()) {
    alarmTimeFile = SD.open(alarmTimeFileName, FILE_WRITE);
    if (alarmTimeFile && alarmTimeFile.size() < MAX_FILE_SIZE) {
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
    Serial.println("Enter a day of week integer from toneMelodyAlarmClock/weekdays.h. This");
    Serial.println("should be i.e. the day of the the next 8:00 AM occurrence.");
    promptedForDay = true;
  }
  if (Serial.available()) {
    alarmDayFile = SD.open(alarmDayFileName, FILE_WRITE);
    if (alarmDayFile && alarmDayFile.size() < MAX_FILE_SIZE) {
      int newDay = Serial.parseInt();
      if (newDay > 0) {
        alarmDayFile.println(newDay);
        Serial.println(newDay);
        daySet = true;
      }
    }
    alarmDayFile.close();
  }
}

void sendTimeToOtherArduino() {
  if (SD.exists(alarmTimeFileName)) {
    alarmTimeFile = SD.open(alarmTimeFileName);
    if (alarmTimeFile) {
        while (alarmTimeFile.available()) {
          Serial1.write(alarmTimeFile.read());
        }
        alreadySentTimeToSerial1 = true;
    }
    alarmTimeFile.close();
  }
}

void sendDayToOtherArduino() {
  if (SD.exists(alarmDayFileName)) {
    alarmDayFile = SD.open(alarmDayFileName);
    if (alarmDayFile) {
        while (alarmDayFile.available()) {
          Serial1.write(alarmDayFile.read());
        }
        alreadySentDayToSerial1 = true;
    }
    alarmDayFile.close();
  }
}

void listenForTriggerFromOtherArduino() {
  if (Serial1.available()) {
    String shouldGo = Serial1.readString();
    Serial.println(shouldGo);
    if (shouldGo.equals("timeplease")) {
      alreadySentTimeToSerial1 = false;
    } else if (shouldGo.equals("dayplease")) {
      alreadySentDayToSerial1 = false;
    }
  }
}