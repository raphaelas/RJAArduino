#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
String fileName = "alarmset.txt";
String alarmDayFileName = "alarmday.txt";
File alarmTimeFile;
File alarmDayFile;
int MAX_FILE_SIZE = 12;

bool alreadySentTimeToSerial1 = false;
bool alreadySentDayToSerial1 = false;
bool timeSet = false;
bool daySet = false;


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
  sendTimeToOtherArduino();
  listenForTriggerFromOtherArduino();
}

void initializeSdCard() {
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
}

void setUpSerialMonitors() {
  Serial1.begin(38400);
  Serial.begin(9600);
  delay(3000);
}

void writeNewTimeToFile() {
  Serial.println("Enter a time");
  if (Serial.available()) {
    alarmTimeFile = SD.open(fileName, FILE_WRITE);
    if (alarmTimeFile && alarmTimeFile.size() < MAX_FILE_SIZE) {
      long newTime = Serial.parseInt();
      if (newTime > 0) {
        alarmTimeFile.println(newTime);
        timeSet = true;
      }
    }
    alarmTimeFile.close();
  }
}

void writeNewDayToFile() {
  Serial.println("Enter a day");
  if (Serial.available()) {
    alarmDayFile = SD.open(alarmDayFileName, FILE_WRITE);
    if (alarmDayFile && alarmDayFile.size() < MAX_FILE_SIZE) {
      int newDay = Serial.parseInt();
      if (newDay > 0) {
        alarmDayFile.println(newDay);
        daySet = true;
      }
    }
  }
}

void sendTimeToOtherArduino() {
  if (!alreadySentTimeToSerial1 && SD.exists(fileName)) {
    alarmTimeFile = SD.open(fileName);
    if (alarmTimeFile) {
        while (alarmTimeFile.available()) {
          Serial1.write(alarmTimeFile.read());
        }
        alreadySentTimeToSerial1 = true;
    }
    alarmTimeFile.close();
  }
}

void listenForTriggerFromOtherArduino() {
  if (Serial1.available()) {
    String shouldGo = Serial1.readString();
    if (shouldGo.equals("go")) {
      alreadySentTimeToSerial1 = false;
    }
  }
}
