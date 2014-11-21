/*
 *
 * Piotr Kluch, 165436
 * Using Photoresistors (LDRs) and PWM units with Arduino
 *
 */
 
 // Pin assignments
 int LDR = 0;
 int LED = 9;
 
 // Initialize the serial port
 // and declare inputs and outputs
 void setup() {
   pinMode(LDR, INPUT);
   pinMode(LED, OUTPUT);
   Serial.begin(9600);
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
void setPWMValue(int led, int brightness) {
   analogWrite(led, brightness);
}

 
// Returns the analog value from input connected to the LDR
// Arudino contains 10-bit, 8 channels digital converter
// It will map voltages between 0 and 5 into integer values
// between 0 and 1023. This yields the resolution between
// redings of: 5 volts / 1024 units or 0.0049 volts (4.9mV) per unit.
// The input range can be changed using analogReference().
// It takes about 100 microseconds (0.0001s) to read an analog input.
// The maximum reading rate is about 10,000 times a second.
int readPhotocellValue(int LDR) {
  return analogRead(LDR);
}
 
int processPhotocellValue(int v) {
   // Interperet photocell value
   // TODO
   if (v<100) 
     v=0;
   if (v>100 && v<400) 
     v=v-100;
   if (v>400) 
     v=255;
   //Inverted function
   //v=255-v;
   if (v<0) 
     v=0;
   return v;
}

 // Read from the analog input connected to the LDR
 // then set the diode or motor PWM value
 // and print values to the serial port.
 // the delay is only to avoid sending so much data
 // as to make it unreadable.
 void loop() {
   
   // Read the photocell value
   int v = readPhotocellValue(LDR);
   
   // Process data
   v = processPhotocellValue(v);
   
   // Set the diode using PWM
   setPWMValue(LED, v);
   
   // Print to serial
   Serial.println(v);
   
   // Delay between each loop cycle
   delay(100);
 }
