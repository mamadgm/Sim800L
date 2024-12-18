#ifndef SIM800CCCC_C
#define SIM800CCCC_C

#include "Arduino.h"

const int PWRKEY = 12;  // GPIO4 for power control

class MySim {
 private:
 public:
  MySim();
  void begin();
  int16_t wait(char* Values, uint16_t timeout);
  void updateSerial();
  void sendSMS(String text);
  void checkSMS(String text);
  String phoneNumber = "9016888626";  // Enter your phone number here
  String message = "hi";
};

extern MySim mob;
#endif
