#ifndef MYLCD_H_H
#define MYLCD_H_H

// Constants

#include "Arduino.h"

class mylcd {
 private:
  /* data */
 public:
  // Enter your message here
  mylcd(/* args */);
  void begin();
  void Off();
  void Menu(String p0);
  void Show(String p0, String p1, String p2, String temp, String humy,
            String air);
  void GSMlog(String what, String where);
};

extern mylcd dis;

#endif