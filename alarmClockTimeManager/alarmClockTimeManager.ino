#include <SPI.h>
#include <SD.h>

const int CHIP_SELECT_PIN = 4;
String alarmTimeFileName = "alarmset.txt";
String alarmDayFileName = "alarmday.txt";
const int FILE_COUNT = 2;
const String FILES_TO_REMOVE[] = {alarmTimeFileName, alarmDayFileName};
const int MAX_FILE_SIZE = 12;
const int SERIAL_MONITOR_STARTUP_DELAY = 3000;
const int BUILTIN_LED_ON_TIME = 10;

bool alreadySentTimeToSerial1 = true;
bool alreadySentDayToSerial1 = true;
bool timeSet = false;
bool daySet = false;
bool promptedForTime = false;
bool promptedForDay = false;

void setup() {
  setUpSerialCommunicators();
  initializeSdCard();
  pinMode(LED_BUILTIN, OUTPUT);
  if (Serial) {
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

void loop() {
  if (Serial && !timeSet) {
    writeNewTimeToFile();
  } else if (Serial && !daySet) {
    writeNewDayToFile();
  }
  listenForTriggerFromOtherArduino();
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
    Serial.println("Card failed, or not present");
    while (1);
  }
}

void setUpSerialCommunicators() {
  Serial1.begin(38400);
  Serial.begin(9600);  // Interestingly, this line might not be needed.
  delay(SERIAL_MONITOR_STARTUP_DELAY);
}

void writeNewTimeToFile() {
  if (!promptedForTime && !SD.exists(alarmTimeFileName)) {
    Serial.println("python /home/raphaelastrow/Arduino/timeuntilalarm.py");
    Serial.println("Enter a time:");
    promptedForTime = true;
  }
  if (Serial.available()) {
    File alarmTimeFile = SD.open(alarmTimeFileName, FILE_WRITE);
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
    Serial.println("Enter a day of week number from toneAlarmClock/weekdays.h. This");
    Serial.println("should be i.e. the day of the the next 8:00 AM occurrence.");
    Serial.println("timeuntilalarm.py prints out the correct day to enter.");
    promptedForDay = true;
  }
  if (Serial.available()) {
    File alarmDayFile = SD.open(alarmDayFileName, FILE_WRITE);
    if (alarmDayFile && alarmDayFile.size() < MAX_FILE_SIZE) {
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
    digitalWrite(LED_BUILTIN, HIGH);
    delay(BUILTIN_LED_ON_TIME);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println(requestFromOtherArduino);
    if (requestFromOtherArduino.equals("timeplease")) {
      alreadySentTimeToSerial1 = false;
    } else if (requestFromOtherArduino.equals("dayplease")) {
      alreadySentDayToSerial1 = false;
    }
  }
}
