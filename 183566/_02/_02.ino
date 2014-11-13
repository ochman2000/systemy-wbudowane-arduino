/*
 * Ten program odlicza 10 sekund i wyświetla tylko te cyfry, które nie są zerem.
 *
 * 2014-11-09, Warszawa, Polska
 * Systemy Wbudowane Politechnika Łódzka
 * Łukasz Ochmański 183566
 */

#include <Wire.h>
#include <TWIDisplay.h>

#define SLAVE_ADDR 0x12

void writeCyfra(int val);
TWIDisplay disp(SLAVE_ADDR);

void setup()
{
  Wire.begin();
  
  Serial.begin(9600);
  Serial.println("Serial Display Test");

  disp.clear();
  disp.setBrightness(255);
}

void loop()
{
  disp.setBrightness(255);
  for (int i = 1000; i>0; i--) {
    writeCyfra(i);
    disp.setDots(0,1,0,0);
    delay(10);
  }
  
  disp.writeStr("SIUP");

  for (int i = 1000; i>0; i--) {
    int a = i%50;
    if (a>0 && a<25)
      disp.setBrightness(255);
    else
      disp.setBrightness(0);
    delay(2);
  }    
}

void writeCyfra(int val)
{
  char m_data[4];
  
  m_data[3] = val % 10;
  val /= 10;
  m_data[2] = val % 10;
  val /= 10;
  m_data[1] = val % 10;
  val /= 10;
  m_data[0] = val % 10;
  
  disp.clear();
  Wire.beginTransmission(SLAVE_ADDR);
  for (int i = 0; i <= 3; i++) {
    if (m_data[i]==0) {
      Wire.write(' ');
    }
    else {
      Wire.write(m_data[i]);
    }
  }
  Wire.endTransmission();
}
