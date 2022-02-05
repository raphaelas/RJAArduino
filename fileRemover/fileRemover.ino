#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
String fileName = "alarmset.txt";

void printTheFileBeforeDeletingIt() {
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

void removeTheFile() {
  bool removalResult = SD.remove(fileName);
  Serial.println("Removal result: ");
  Serial.println(removalResult);    
  if (!SD.exists(fileName)) {
    Serial.println(fileName + " does not exist anymore.");
  }
}

void initializeSdCard() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");
}

void setUpSerialMonitor() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

void setup() {
  setUpSerialMonitor();
  initializeSdCard();
  if (SD.exists(fileName)) {
    Serial.println(fileName + " exists.");
    printTheFileBeforeDeletingIt();
    removeTheFile();
  } else {
    Serial.println(fileName + " does not exist.");
  }
}

void loop() {}
