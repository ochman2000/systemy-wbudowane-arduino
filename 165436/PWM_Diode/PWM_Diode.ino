/*
 Fade
 
 This example shows how to fade an LED on pin 9
 using the analogWrite() function.
 
 This example code is in the public domain.
 */

int led = 9;           // the pin that the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup()  { 
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
} 

// Input range: 0 - 255
// Writes an analog value (PWM wave) to a pin
// The pin will generate a steady square wave of the
// specified duty cycle until next call to this function
// Frequency is approx 490 Hz
// On Ardu Uno Pin 5 and 6 are 980 Hz
// analogWrite() works on pin 3, 5, 6, 9, 10, 11 normally with ATmega328
// pinMode() needs to be called before use of this function.
// The analogWrite function has nothing to do with the analog pins 
// or the analogRead function.
void setPWMAmount(int led, int brightness) {
   analogWrite(led, brightness);
}

// the loop routine runs over and over again forever:
void loop()  { 
  // set the brightness of pin 9:
  setPWMAmount(led, brightness);    

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade: 
  if (brightness == 0 || brightness == 255) {
    fadeAmount = -fadeAmount ; 
  }     
  // wait for 30 milliseconds to see the dimming effect    
  delay(30);                            
}

