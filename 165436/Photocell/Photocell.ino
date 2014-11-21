/*
 *
 * Piotr Kluch, 165436
 * Using Photoresistors (LDRs) with Arduino
 *
 */
 
 // Pin assignments
 int LDR = 0;
 
 // Initialize the serial port
 // and declare inputs and outputs
 void setup() {
   pinMode(LDR, INPUT);
   Serial.begin(9600);
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
 
 // Read from the analog input connected to the LDR
 // and print the value to the serial port.
 // the delay is only to avoid sending so much data
 // as to make it unreadable.
 void loop() {
   
   int v = readPhotocellValue(LDR);
   
   // Print to serial
   Serial.println(v);
   
   // Delay between each loop cycle
   delay(100);
 }
