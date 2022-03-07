#include <MKRWAN.h>
#include <SPI.h>
#include <SD.h>
#include "alarmClockTimeManagerGlobalVariables.h"
#include "sdcardconstants.h"
#include "alarmclocktimemanagertimeconstants.h"
#include "credentials.h"

LoRaModem modem;

const String appEui = "0000000000000000";
const int LORA_PORT = 3;
const char * TUNE_REQUEST = "tuneplease";
bool isLoraConnected = false;
long lastLoRaAction = -SECONDS_IN_MINUTE * 2;

const int TIME_INCREASE_SWITCH = 7;
const int TIME_DECREASE_SWITCH = 9;
const int EXTERNAL_LED = 12;




void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TIME_INCREASE_SWITCH, INPUT);
  pinMode(TIME_DECREASE_SWITCH, INPUT);
  pinMode(EXTERNAL_LED, OUTPUT);
  setUpSerialCommunicators();
  setUpLoRa();
}


long negativeSafeModulo(long k, long n) {
    return ((k %= n) < 0) ? k + n : k;
}

void setUpLoRa() {
  if (!modem.begin(US915)) {
    Serial.println("Failed to start modem. Did you select your region?");
    while (true) {
      alternateLight(LED_BUILTIN, ONE_SECOND * 2);
    }
  };
}

void alternateLight(int ledPin, int lightTime) {
  digitalWrite(ledPin, HIGH);
  delay(lightTime);
  digitalWrite(ledPin, LOW);
  delay(lightTime);  
}

void loop() {
  if (!isLoraConnected && !loRaActionRecentlyPerformed()) {
    doConnectLoRaWAN();
  }
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
  checkBothSwitchesPressedState();
  checkTimeIncreaseOnlySwitchState();
  checkTimeDecreaseOnlySwitchState();
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
  if (!promptedForTime && !SD.exists(ALARM_TIME_FILE_NAME)) {
    Serial.println("python /home/$USER/Arduino/timeuntilalarm.py");
    Serial.println("Enter a time:");
    promptedForTime = true;
  }
  if (Serial.available()) {
    File alarmTimeFile = SD.open(ALARM_TIME_FILE_NAME, FILE_WRITE);
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
  if (!promptedForDay && !SD.exists(ALARM_DAY_FILE_NAME)) {
    Serial.println("Enter a day of week number from toneAlarmClock/weekdays.h. This");
    Serial.println("should be i.e. the day of the the next 8:00 AM occurrence.");
    Serial.println("timeuntilalarm.py prints out the correct day to enter.");
    promptedForDay = true;
  }
  if (Serial.available()) {
    File alarmDayFile = SD.open(ALARM_DAY_FILE_NAME, FILE_WRITE);
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
  if (SD.exists(ALARM_TIME_FILE_NAME)) {
    File alarmTimeFile = SD.open(ALARM_TIME_FILE_NAME);
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
  if (SD.exists(ALARM_DAY_FILE_NAME)) {
    File alarmDayFile = SD.open(ALARM_DAY_FILE_NAME);
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
    blinkLight(LED_BUILTIN);
    Serial.println(requestFromOtherArduino);
    if (requestFromOtherArduino.equals("timeplease")) {
      alreadySentTimeToSerial1 = false;
    } else if (requestFromOtherArduino.equals("dayplease")) {
      alreadySentDayToSerial1 = false;
    }
  }
}

void blinkLight(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(BUILTIN_LED_ON_TIME);
  digitalWrite(ledPin, LOW);
}

bool loRaActionRecentlyPerformed() {
  bool result = (millis() / ONE_SECOND) < (lastLoRaAction + (SECONDS_IN_MINUTE * 2));
  if (result) {
    alternateLight(EXTERNAL_LED, ONE_SECOND);
  }
  return result;
}

void doConnectLoRaWAN() {
  Serial.println("Connecting via LoRaWAN.");
  long startMilliseconds = millis();
  digitalWrite(EXTERNAL_LED, HIGH);
  isLoraConnected = modem.joinOTAA(appEui, appKey, JOIN_TIMEOUT);
  if (!isLoraConnected) {
    Serial.println("Connection via LoRaWAN failed."); 
  } else {
    Serial.println("Connected.");
    Serial.print("Connection seconds: ");
    Serial.println((millis() - startMilliseconds) / ONE_SECOND);
    lastLoRaAction = millis() / ONE_SECOND;
    blinkLight(LED_BUILTIN);
    digitalWrite(EXTERNAL_LED, LOW);
  }
}

void doSendMessage() {
  Serial.println("Sending message.");
  digitalWrite(EXTERNAL_LED, HIGH);
  modem.setPort(LORA_PORT);
  modem.beginPacket();
  modem.print(TUNE_REQUEST);
  int err = modem.endPacket(false);
  if (err > 0) {
    Serial.println("Message sent correctly.");
    lastLoRaAction = millis() / ONE_SECOND;
    Serial.println("Retry allowed in 2 minutes.");
    blinkLight(LED_BUILTIN);
  } else {
    Serial.println("Error sending message.");
  }
  Serial.print("Send code: ");
  Serial.println(err);
  digitalWrite(EXTERNAL_LED, LOW);
}

void checkBothSwitchesPressedState() {
  if (!loRaActionRecentlyPerformed() && switchPressed(TIME_INCREASE_SWITCH)
      && switchPressed(TIME_DECREASE_SWITCH)) {
    doSendMessage();
  }
}

void checkTimeIncreaseOnlySwitchState() {
  while (switchPressed(TIME_INCREASE_SWITCH) && !switchPressed(TIME_DECREASE_SWITCH)) {
    timeChangeAmount++;
  }
  if (timeChangeAmount > 0) {
    changeTheTime(timeChangeAmount);
  }
}

void checkTimeDecreaseOnlySwitchState() {
  while (switchPressed(TIME_DECREASE_SWITCH) && !switchPressed(TIME_INCREASE_SWITCH)) {
    timeChangeAmount--;
  }
  if (timeChangeAmount < 0) {
    changeTheTime(timeChangeAmount);
  }
}

bool switchPressed(int switchPin) {
  return digitalRead(switchPin) == HIGH;
}

void changeTheTime(int theTimeChangeAmount) {
  long oldTime = readOldTime();
  if (oldTime > 0) {
    removeFile(ALARM_TIME_FILE_NAME);
    long newTime = oldTime + (ONE_MINUTE * 10 * theTimeChangeAmount);
    changeTime(negativeSafeModulo(newTime, ONE_DAY));
    if (newTime >= ONE_DAY) {
      shiftDay(1);
    } else if (newTime < 0) {
      shiftDay(-1);
    }
    blinkLight(LED_BUILTIN);
  }
  timeChangeAmount = 0;
}

void shiftDay(int dayDelta) {
  int oldDay = readOldDay();
  if (oldDay > 0) {
    removeFile(ALARM_DAY_FILE_NAME);
    int newDay = oldDay + dayDelta;
    int newDayWrappedAround = (negativeSafeModulo((newDay - 1), DAYS_IN_WEEK)) + 1;
    changeDay(newDayWrappedAround);   
  }
}

void changeDay(int newDay) {
  File alarmDayFile = SD.open(ALARM_DAY_FILE_NAME, FILE_WRITE);
  if (alarmDayFile) {
    alarmDayFile.println(newDay);
  }
  alarmDayFile.close();
}

void changeTime(long newTime) {
  File alarmTimeFile = SD.open(ALARM_TIME_FILE_NAME, FILE_WRITE);
  if (alarmTimeFile) {
    alarmTimeFile.println(newTime);
  }
  alarmTimeFile.close();
}

long readOldTime() {
  long oldTime;
  File timeFile = SD.open(ALARM_TIME_FILE_NAME);
  while (timeFile.available()) {
    long parsedNumberBuffer = timeFile.parseInt();
    if (parsedNumberBuffer > 0) {
      oldTime = parsedNumberBuffer;
    }
  }
  timeFile.close();
  return oldTime;
}

long readOldDay() {
  int oldDay;
  File dayFile = SD.open(ALARM_DAY_FILE_NAME);
  while (dayFile.available()) {
    int parsedNumberBuffer = dayFile.parseInt();
    if (parsedNumberBuffer > 0) {
      oldDay = parsedNumberBuffer;
    }
  }
  dayFile.close();
  return oldDay;
}
