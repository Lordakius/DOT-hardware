#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

#include <WiFiSecrets.h>

ESP8266WiFiMulti WiFiMulti;

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (4)
#define PN532_SS   (5)
#define PN532_MISO (0)

// Define RBG LED pins
#define RED (14)
#define GREEN (13)
#define BLUE (12)

// Define Backend url

#define BACKEND_URL ("http://jigsaw.w3.org/HTTP/connection.html")

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Use this line for a breakout with a software SPI connection (recommended):
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

void blink(int colour, int duration) {
  digitalWrite(colour, HIGH);
  delay(duration);
  digitalWrite(colour, LOW);
  delay(duration);
}

void setup(void) {
  // show startup phase by lighting up blue
  digitalWrite(BLUE, HIGH);
  
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();

  // setup LED pins
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  Serial.println("Waiting for an ISO14443A Card ...");

  // starting WIFI
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SECRET_SSID, SECRET_PASSWORD);

  // wait for Wifi connection
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    delay(100);
  }

  //ending start up phase
  digitalWrite(BLUE, LOW);
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // show the user the start of the sending process
    digitalWrite(BLUE, HIGH);
    
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    // post uid to backend
    if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, BACKEND_URL)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          digitalWrite(BLUE, LOW);
          blink(GREEN, 500);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        digitalWrite(BLUE, LOW);
        blink(RED, 500);
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
      digitalWrite(BLUE, LOW);
      blink(RED, 500);
    }
  }
    
  }
  else {
    Serial.println("Unable to read card!");
    blink(RED, 500);
  }
}
