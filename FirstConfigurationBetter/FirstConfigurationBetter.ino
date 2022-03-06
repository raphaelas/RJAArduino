#include <MKRWAN.h>
#define ONE_SECOND   1000
LoRaModem modem;

// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

String appEui = "0000000000000000";
String appKey = "APP_KEY_GOES_HERE";
bool connected = false;
const int RETRY_SWITCH = 7;
int mode = 1;

void setup() {
  pinMode(RETRY_SWITCH, INPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Welcome to MKR WAN 1300/1310 first configuration sketch");
  Serial.println("Register to your favourite LoRa network and we are ready to go!");
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(US915)) {
    Serial.println("Failed to start module");
    while (true);
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  if (modem.version() != ARDUINO_FW_VERSION) {
    Serial.println("Please make sure that the latest modem firmware is installed.");
    Serial.println("To update the firmware upload the 'MKRWANFWUpdate_standalone.ino' sketch.");
  }
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
}

void loop() {
  while (!connected) {
    Serial.println("Waiting before trying again.");
    delay(long(ONE_SECOND) * 90);
    Serial.println("Connecting.");
    connected = modem.joinOTAA(appEui, appKey);
    if (!connected) {
      Serial.println("Something went wrong. Are you indoors? Move near a window and retry."); 
    }
  }
  doSendMessage();
}

void doSendMessage() {
  Serial.println("Connected. Waiting 5 seconds.");
  Serial.println(millis() / ONE_SECOND);
  delay(ONE_SECOND * 5);
  Serial.println("Sending message.");
  modem.setPort(3);
  modem.beginPacket();
  modem.print("HeLoRA world!");
  int err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
  }
  Serial.println(err);
  delay(long(ONE_SECOND) * 120);
  Serial.println("Now you can try again.");
  while (digitalRead(RETRY_SWITCH) == LOW);
}
