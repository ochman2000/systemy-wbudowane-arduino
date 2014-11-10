/*
 * Ten program odlicza 10 sekunk i wyświetla tylko te cyfry, które nie są zerem.
 *
 * 2014-11-09, Warszawa, Polska
 * Systemy Wbudowane Politechnika Łódzka
 * Łukasz Ochmański 183566
 */

#include <Wire.h>
#include <TWIDisplay.h>

#define SLAVE_ADDR 0x12

TWIDisplay disp(SLAVE_ADDR);

void setup()
{
  Wire.begin();
  
  Serial.begin(9600);
  Serial.println("Serial Display Test");

  disp.setRotateMode();
  disp.clear();
  disp.setBrightness(255);
}

void loop()
{
  for (int i=0; i<80; i++) {
    Wire.beginTransmission(0x12);
    Wire.write(i);
    Wire.write(i);
    Wire.write(i);
    Wire.write(i);
    Wire.endTransmission();
    delay(1000);
  }
}
