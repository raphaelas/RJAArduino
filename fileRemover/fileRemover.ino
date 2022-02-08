#include <SPI.h>
#include <SD.h>

const int CHIP_SELECT_PIN = 4;
const int FILE_COUNT = 2;
const String FILES_TO_REMOVE[FILE_COUNT] = {"alarmset.txt", "alarmday.txt"};

void setup() {
  setUpSerialMonitor();
  initializeSdCard();
  for (int file = 0; file < FILE_COUNT; file++) {
    String fileName = FILES_TO_REMOVE[file];
    if (SD.exists(fileName)) {
      Serial.println(fileName + " exists.");
      printTheFileBeforeDeletingIt(fileName);
      removeTheFile(fileName);
    } else {
      Serial.println(fileName + " does not exist.");
    }
  }
}

void loop() {}

void printTheFileBeforeDeletingIt(String fileName) {
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

void removeTheFile(String fileName) {
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

void setUpSerialMonitor() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}
