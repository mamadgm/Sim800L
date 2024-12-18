#include "mylcd.h"

#include <LiquidCrystal_PCF8574.h>
// LCD setup
LiquidCrystal_PCF8574 lcd(
    0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

mylcd dis;

mylcd::mylcd(/* args */) {}

void mylcd::Show(String p0, String p1, String p2, String temp, String humy,
                 String air) {
                  lcd.clear();
                  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  lcd.print(p0);
  lcd.setCursor(5, 0);
  lcd.print(p1);
  lcd.setCursor(10, 0);
  lcd.print(p2);
  lcd.setCursor(0, 1);
  lcd.print(temp);
  lcd.setCursor(5, 1);
  lcd.print(humy);
  lcd.setCursor(10, 1);
  lcd.print(air);
}

void mylcd::GSMlog(String what, String where) {
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  lcd.print(what);
  lcd.setCursor(0, 1);
  lcd.print(where);
}

void mylcd::Off() {
  lcd.clear();
  delay(500);
  lcd.setBacklight(0);
}
void mylcd::Menu(String p0) {
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  lcd.print(p0);
}
void mylcd::begin() {
  lcd.begin(16, 2);
  delay(100);
}