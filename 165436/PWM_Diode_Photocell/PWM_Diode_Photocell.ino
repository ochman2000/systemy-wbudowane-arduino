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
   Serial.begin(9600);
 }
 
 // Read from the analog input connected to the LDR
 // then set the diode or motor PWM value
 // and print values to the serial port.
 // the delay is only to avoid sending so much data
 // as to make it unreadable.
 void loop() {
   
   // Read the photocell value
   int v = analogRead(LDR);
   
   // Interperet photocell value
   // TODO
   if (v<100) v=0;
   if (v>100 && v<400) 
     v=v-100;
   if (v>400) 
     v=255;
   //v=255-v;
   if (v<0) v=0;
   
   // Set the diode using PWM
   analogWrite(LED, v);
   
   // Print to serial
   Serial.println(v);
   
   // Delay between each loop cycle
   delay(100);
 }
