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
 
 // Read from the analog input connected to the LDR
 // and print the value to the serial port.
 // the delay is only to avoid sending so much data
 // as to make it unreadable.
 void loop() {
   
   // Read the photocell value
   int v = analogRead(LDR);
   
   // Print to serial
   Serial.println(v);
   
   // Delay between each loop cycle
   delay(100);
 }
