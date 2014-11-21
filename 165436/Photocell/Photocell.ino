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
