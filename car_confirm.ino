#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the MFRC522 class
MFRC522::MIFARE_Key key;

byte nuidPICC[4];
unsigned long table = 0;

void setup() {
  Wire.begin(2);             // Set the I2C slave address to 2
  Wire.onRequest(sendData);  // Register the event to send data
  Wire.onReceive(receiveEvent); // Register to receive commands
  Serial.begin(9600);        // Initialize Serial Monitor
  SPI.begin();               // Init SPI bus
  rfid.PCD_Init();           // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;   // Initialize key with default value
  }
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  // Check if a new card is detected
  if (memcmp(rfid.uid.uidByte, nuidPICC, 4) != 0) {
    Serial.println(F("A new card has been detected."));
    memcpy(nuidPICC, rfid.uid.uidByte, 4);

    Serial.print(F("The NUID tag in decimal is: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    // Check for specific UID values and set the table value
    if (rfid.uid.uidByte[0] == 83 && rfid.uid.uidByte[1] == 246 &&
        rfid.uid.uidByte[2] == 176 && rfid.uid.uidByte[3] == 247) {
      table = 1;
    } else if (rfid.uid.uidByte[0] == 173 && rfid.uid.uidByte[1] == 71 &&
               rfid.uid.uidByte[2] == 125 && rfid.uid.uidByte[3] == 0) {
      table = 2;
    }
    else if (rfid.uid.uidByte[0] == 96 && rfid.uid.uidByte[1] == 246 &&
               rfid.uid.uidByte[2] == 64 && rfid.uid.uidByte[3] == 33) {
      table = 3; 
               }
    else {
      table = 0;
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(200);
  
}

/**
 * Helper routine to print a byte array as decimal values
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i]);
    if (i < bufferSize - 1) Serial.print(", ");
  }
}

/**
 * Function to send data to the master
 */
void sendData() {
  Wire.write(table);  // Send the value of `table` to the master
  Serial.println(table);
  delay(30);
}

/**
 * Function to receive commands from the master
 */
void receiveEvent(int howMany) {
  if (Wire.available()) {
    char command = Wire.read();
    if (command == 'C') {
      table = 0;  // Reset the table value
      memset(nuidPICC, 0, sizeof(nuidPICC));
      Serial.println("Table value reset by master command.");
      delay(100);
    }
  }
}
