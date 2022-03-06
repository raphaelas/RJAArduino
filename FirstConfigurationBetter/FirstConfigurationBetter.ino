#include <MKRWAN.h>
#include "credentials.h"

#define ONE_SECOND   1000
#define ONE_MINUTE   long(ONE_SECOND) * 60
#define JOIN_TIMEOUT ONE_MINUTE * 3

LoRaModem modem;

const String appEui = "0000000000000000";

const int RETRY_SWITCH = 7;
const int mode = 1;
const char ARDUINO_CURRENT_VERSION[] = "ARD-078 1.2.3";

const int LORA_PORT = 3;
const char * HELLO_WORLD_MESSAGE = "HelLoRA world!";

bool isConnected = false;

// Remember to 

void setup() {
  pinMode(RETRY_SWITCH, INPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("MKR WAN 1300/1310 LoRaWAN activation sketch.");
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(US915)) {
    Serial.println("Failed to start module. Did you select your region?");
    while (true);
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  if (modem.version() != ARDUINO_CURRENT_VERSION) {
    Serial.println("Please make sure that the latest modem firmware is installed.");
    Serial.println("To update the firmware upload the 'MKRWANFWUpdate_standalone.ino' sketch.");
  }
  Serial.print("Your device EUI (DevEUI) is: ");
  Serial.println(modem.deviceEUI());
}

void loop() {
  while (!isConnected) {
    doConnect();
  }
  doSendMessage();
}

void doConnect() {
  Serial.println("Connecting/activating.");
  long startMilliseconds = millis();
  isConnected = modem.joinOTAA(appEui, appKey, JOIN_TIMEOUT);
  if (!isConnected) {
    Serial.println("Connection/activation failed. Are you indoors? Move near a window and retry."); 
  } else {
    Serial.println("Connected.");
    Serial.print("Connection seconds: ");
    Serial.println((millis() - startMilliseconds) / ONE_SECOND);
    Serial.println("Waiting 2 minutes.");
    delay(ONE_MINUTE * 2);
  }
}

void doSendMessage() {
  Serial.println("Sending message.");
  modem.setPort(LORA_PORT);
  modem.beginPacket();
  modem.print(HELLO_WORLD_MESSAGE);
  int err = modem.endPacket(false);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
  }
  Serial.print("Send code: ");
  Serial.println(err);
  Serial.println("Waiting 2 minutes before retry allowed.");
  delay(ONE_MINUTE * 2);
  Serial.println("Now you can try again.");
  while (digitalRead(RETRY_SWITCH) == LOW);
}
