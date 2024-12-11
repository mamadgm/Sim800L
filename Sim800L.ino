#include <SoftwareSerial.h>

// #include "src/sim800c/sim.h"
const int PWRKEY = 4; // GPIO4 for power control
String phoneNumber = "+989016888626"; // Enter your phone number here
String message = "hi"; // Enter your message here
int16_t wait(char* Values, uint16_t timeout);
SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
  pinMode(PWRKEY, OUTPUT);
  digitalWrite(PWRKEY, HIGH); // Ensure PWRKEY is HIGH initially

  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("hello Initializing...");
  delay(1000);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.indexOf("SENDSMS") > -1) {
      Serial.println("Detect");
      sendSMS();
    } else {
      mySerial.println(input);
      updateSerial();
    }
  }

  // updateSerial();
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read()); // Forward what Software Serial received to Serial Port
  }
}

void sendSMS() {
  // Check if the module is on
  mySerial.println("AT");
  int16_t response = wait("OK,ERROR", 5000);
  updateSerial();
  if (response == 0) {
    // Module is on, turn it off
    Serial.println("Turning off the module...");
    digitalWrite(PWRKEY, LOW);
    delay(1000);
    updateSerial();
    digitalWrite(PWRKEY, HIGH);
    delay(3000); // Wait for the module to turn off
    updateSerial();
  }

  // Turn on the module
  Serial.println("Turning on the module...");
  digitalWrite(PWRKEY, LOW);
  delay(1000);
  updateSerial();
  digitalWrite(PWRKEY, HIGH);
  delay(3000); // Wait for the module to turn on
  updateSerial();
  // Wait for SMS ready
  response = wait("SMS Ready,ERROR", 10000);
  updateSerial();
  if (response != 0) {
    Serial.println("Failed to initialize SMS mode.");
    return;
  }
  updateSerial();
  // Send SMS
  mySerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  updateSerial();
  mySerial.print("AT+CMGS=\"");
  mySerial.print(phoneNumber);
  mySerial.println("\"");
  delay(1000);
  response = wait(">,ERROR", 1000);
  updateSerial();
  mySerial.print(message); // Message content
  delay(1000);
  updateSerial();
  mySerial.write(26); // Send Ctrl+Z to send the SMS
  delay(5000);
  updateSerial();

  // Turn off the module
  Serial.println("Turning off the module...");
  digitalWrite(PWRKEY, LOW);
  delay(3000);
  digitalWrite(PWRKEY, HIGH);
  delay(3000);
}


int16_t wait(char* Values, uint16_t timeout) {

  uint16_t Length = strlen(Values);

  char InputBuffer[Length + 1];
  strcpy(InputBuffer, Values);
  char CompareBuffer[Length + 1];
  memset(CompareBuffer,0,sizeof(CompareBuffer));

  uint16_t Quantity = 1;
  
  for (int16_t n = 0; n < Length; n++) {
    if (InputBuffer[n] == ',')
      Quantity++;
  }

  char* InputTokens[Quantity];
  memset(InputTokens, 0, sizeof(InputTokens));
  char* CompareTokens[Quantity];
  memset(CompareTokens, 0, sizeof(CompareTokens));

  InputTokens[0] = InputBuffer;
  CompareTokens[0] = CompareBuffer;

  uint16_t TokenPosition = 1;
  for (int16_t n = 0; n < Length; n++) {
    if (InputBuffer[n] == ',') {
      InputBuffer[n] = 0;
      InputTokens[TokenPosition] = &InputBuffer[n + 1];
      CompareTokens[TokenPosition] = &CompareBuffer[n + 1];
      TokenPosition++;
    }
  }

  uint64_t timer = millis();
  char c;

  while (millis() - timer < timeout) {
    while (mySerial.available()) {
      c = mySerial.read();
      Serial.print(c);

      for (int16_t n = 0; n < Quantity; n++) {
        Length = strlen(CompareTokens[n]);
        if (c == InputTokens[n][Length])
          CompareTokens[n][Length] = c;
        else
          memset(CompareTokens[n], 0, Length);

        if (!strcmp(InputTokens[n],CompareTokens[n]))
          return n;
      }
    }
  }
  return -1;
}