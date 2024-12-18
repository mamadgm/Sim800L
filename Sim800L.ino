#include <EEPROM.h>
#include <Keypad.h>
#include <Wire.h>  // https://www.arduino.cc/reference/en/language/functions/communication/wire/

#include "forcedBMX280.h"
#include "header.h"
//
#define BMX_CYCLE 2000  // frequency for BMX280 readout (in ms)
ForcedBME280Float climateSensor = ForcedBME280Float(Wire, 0x76);
int32_t g_temperature;     // current temperature
uint32_t g_pressure;       // current pressure
uint32_t g_humidity;       // current humidity
float g_temperatureFloat;  // current temperature
float g_pressureFloat;     // current pressure
float g_humidityFloat;     // current humidity
//
//
const int sensorPin = A1;            // Analog pin connected to the sensor
const float referenceVoltage = 1.1;  // Internal reference voltage
const float resistorValue = 51.0;    // Resistor value in ohms
const float maxCurrent = 0.0198;     // 20 mA
const float minCurrent = 0.0038;     // 4 mA
const float maxValue = 6.0;          // Maximum value corresponding to 20 mA
const float minValue = 0.0;
const int sesnsorupdatecountnumbers = 10;
int sesnsorupdatecount = 0;
String valueString = "";
float value;
//
const byte ROWS = 4;  // Four rows
const byte COLS = 4;  // Four columns
char keys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                         {'4', '5', '6', 'B'},
                         {'7', '8', '9', 'C'},
                         {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables
unsigned long lastKeyPressTime = 0;
const unsigned long timeout = 20000;
bool inMenu = false;
bool inValueMenu = false;
bool inShow = false;
int valueIndex = 0;
const int predefinedValues[] = {1, 2, 3, 4, 6, 12, 24};
int hourValue = 0;
unsigned long previousMillis = 0;
unsigned long interval;
unsigned long HpreviousMillis = 0;  // Stores last time SMS was checked
const long Hinterval = 150000;
void setup() {
  // set LED as output and turn it off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  mob.begin();
  // Initialize the dis
  dis.begin();
  // Enable internal AREF 1.1V
  analogReference(INTERNAL);
  Wire.begin();
  while (climateSensor.begin()) {
    Serial.println("Waiting for sensor...");

    delay(1000);
  }
  digitalWrite(LED_BUILTIN, LOW);
  // Retrieve values from EEPROM
  retrieveValuesFromEEPROM();
  Serial.println(mob.phoneNumber);
  Serial.println(hourValue);
  Serial.println("hello Initializing...");
  delay(500);
  interval = hourValue * 3600000UL;  // 3600000UL
}

float mymap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void checksms() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    mob.sendSMS("s1 : " + valueString + "\n" +
                "temp : " + String(g_temperatureFloat, 1) + "\n" +
                "airp : " + g_pressure + "\n" + "humy : " + "28");
  }
  // if (currentMillis - HpreviousMillis >= Hinterval) {
  //   HpreviousMillis = currentMillis;
  //   mob.checkSMS("hello" + valueString);
  // }
}
void loop() {
  checksms();
  for (int J = 0; J < 10; J++) {
    //
    int sensorValue = analogRead(sensorPin);
    float voltage = sensorValue * (referenceVoltage / 1023.0);
    float current = voltage / resistorValue;
    value = value + mymap(current, minCurrent, maxCurrent, minValue, maxValue);

    //
    for (int i = 0; i < 100; i++) {
      char key = keypad.getKey();

      if (key) {
        Serial.println(key);
        lastKeyPressTime = millis();
        if (inMenu) {
          handleMenu(key);
        } else if (inValueMenu) {
          handleValueMenu(key);
        } else {
          if (key == 'A') {
            inMenu = true;
            inShow = false;
            mob.phoneNumber = "";
            dis.Menu("Enter Number:");
          } else if (key == 'B') {
            inValueMenu = true;
            inShow = false;
            valueIndex = 0;
            dis.Menu(String(predefinedValues[valueIndex]));
          } else if (key == '*') {
            inShow = false;
            mob.sendSMS("s1 : " + valueString + "\n" +
                        "temp : " + String(g_temperatureFloat, 1) + "\n" +
                        "airp : " + g_pressure + "\n" + "humy : " + "28");

          } else {
            inShow = true;
            dis.Show(valueString, "off", "off", String(g_temperatureFloat, 1),
                     "28%", String(g_pressure));
          }
        }
      }

      delay(1);
    }
    if (millis() - lastKeyPressTime > timeout) {
      inShow = false;
      dis.Off();
    }
  }
  // Serial.println("Before Min " + String(value));
  value = value / 10;
  if (value < 0) {
    value = 0.0;
  }
  // Serial.println("After Min " + String(value));
  climateSensor.takeForcedMeasurement();
  g_pressure = climateSensor.getPressure();
  g_temperatureFloat = climateSensor.getTemperatureCelsiusAsFloat(true);
  valueString = String(value, 1);
  if (inShow) {
    Serial.println("update");
    dis.Show(valueString, "off", "off", String(g_temperatureFloat, 1), "28%",
             String(g_pressure));
  }
}

void handleMenu(char key) {
  if (key == 'B') {
    if (mob.phoneNumber.length() > 0) {
      mob.phoneNumber.remove(mob.phoneNumber.length() - 1);
    }
  } else if (key == 'C') {
    inMenu = false;
    dis.Menu("number saved");
    delay(500);
    dis.Menu(mob.phoneNumber);
    delay(2000);
    savephoneNumberToEEPROM();
  } else if (key == 'D') {
    inMenu = false;
    dis.Menu("Cancelled");
    delay(2000);
  } else if (mob.phoneNumber.length() < 10 && isDigit(key)) {
    mob.phoneNumber += key;
  }
  dis.Menu(mob.phoneNumber);
}

void handleValueMenu(char key) {
  if (key == 'B') {
    valueIndex = (valueIndex + 1) %
                 (sizeof(predefinedValues) / sizeof(predefinedValues[0]));
    dis.Menu(String(predefinedValues[valueIndex]));
  } else if (key == 'C') {
    inValueMenu = false;
    dis.Menu("Value saved");
    delay(500);
    dis.Menu(String(predefinedValues[valueIndex]));
    delay(2000);
    saveValueToEEPROM();
  } else if (key == 'D') {
    inValueMenu = false;
    dis.Menu("Cancelled");
    delay(2000);
  }
}

void savephoneNumberToEEPROM() {
  for (int i = 0; i < 10; i++) {
    EEPROM.write(i, mob.phoneNumber[i]);
  }
}

void saveValueToEEPROM() {
  hourValue = predefinedValues[valueIndex];
  EEPROM.write(10, predefinedValues[valueIndex]);
  interval = hourValue * 3600000UL;
}

void retrieveValuesFromEEPROM() {
  char tempphoneNumber[11];
  for (int i = 0; i < 10; i++) {
    tempphoneNumber[i] = EEPROM.read(i);
  }
  tempphoneNumber[10] = '\0';
  mob.phoneNumber = String(tempphoneNumber);

  hourValue = EEPROM.read(10);
}
