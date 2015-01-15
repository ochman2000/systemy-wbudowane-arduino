/*==================================================================
 | Alarm Countdown Timer 4000 - Final Program
 | systemy-wbudowane-arduino
 | ==========================================
 | 2015-01-10, Łódź, Polska
 | Systemy Wbudowane, Politechnika Łódzka
 |
 | ## Description
 |   This program firstly prints "HI" message on the segment display.
 | Then it waits for the button press in order to start the timer
 | countdown. Before starting the timer you can wave your hand over
 | the snooze button (ultrasonic sensor) to add up more time (1 wave 
 | equals 5 additional seconds). 
 |   After the timer starts, the green LED indicator swit-
 | ches on. You can pause/resume it at any time using the same 
 | button. 
 |   When the T=0 is reached, the alarm goes off. The RGB diode 
 | will start changing color, vibration motor speed will be
 | manipulated using PWM, same with piezoelectric speaker.
 |   You can wave your hand over the snooze button again (sonar 
 | sensor), in order to extend the timer with additional 5 seconds.
 |   In order to turn off the alarm, you need to hover your hand
 | very close to the ultrasonic sensor (approx. 0 to 5 centimeters). 
 | Then the device will reboot itself and the whole process 
 | can be repeated.
 |
 | ## Authors
 | Piotr Kluch, 165436
 | Lukasz Ochmanski, 183566
 | Adam Gruszka, 165409
 |
 | ## Version
 | 1.0
 |
 | ## Structure
 | This program is divided into seven sections.
 | It contains the following:
 | - Libraries
 | - Variables declarations and initializations
 | - Prepare runtime functions and other initialization routines
 | - Runtime functions definitions
 | - Loops
 | - Helper functions
 | - Other / Miscellaneous
 | 
 | ## Pins in use
 | Digital pins in use on the Arduino board: 
 | 7, 13, (segment display), 
 | 2, 3, (sonar sensor),
 | 8, 9, 10, (shift register)
 | 5, 6 (PWM vibration motor and piezo speaker)
 |
 ==================================================================*/

/*******************************************************************
 * Libraries
 *******************************************************************/

// Libraries needed for the segment display (Lukasz)
#include <TimerOne.h>
#include <Wire.h>
#include <TWIDisplay.h>

/*******************************************************************
 * Variables declarations and initializations
 *******************************************************************/

// Settings for segment display (Lukasz)
#define SLAVE_ADDR 0x12
TWIDisplay disp(SLAVE_ADDR);
const int   buttonPin = 7;    // Pin, który zczytuje wartości
const int   ledPin = 13;      // Dioda
int   enabled = HIGH;
int   obecny;
int   ostatni = LOW;
long  czasWcisniecia = 0;
long  minimalnyCzasWcisniecia = 50;
volatile int i = 500;         // Timer initial value 500 equal to 5[s]
void writeCyfra(int val);

// Settings Ultrasonic Sensor (Adam)
const int  trigPin = 2;     // Numer pinu wyzwolenia
const int  echoPin = 3;     // Numer pinu odpowiedzi
long  responseTime;         // Czas trwania sygnału echoPin
int   distance;             // Odległość w centymetrach
int   masimumRange = 200;   // Maksymalna odległość
int   minimumRange = 2;     // Minimalna odległość

// Settings Shifted Register (Piotr)
const int   DS_pin = 8;     // Data pin
const int   STCP_pin = 9;   // Latch pin
const int   SHCP_pin = 10;

// Settings for PWM (Piotr)
const int   vibrationMotorPin = 5;
const int   buzzerPin = 6;

// Other (Piotr)
boolean programFirstRun = true;
void starWarsTheme();
volatile int previousDistance = 0;
boolean snoozeButtonPressed = false;
void(* resetFunc) (void) = 0;

/*******************************************************************
 * Prepare runtime functions and other initialization routines
 *******************************************************************/

void setup() {
  // Segment display
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, enabled);
  // Set a timer of length 10000 microseconds
  // (or 0.01 sec - or 100Hz)
  Timer1.initialize(10000);
  Timer1.attachInterrupt( timerIsr ); // Attach the service routine here
  Wire.begin();

  disp.clear();
  disp.setBrightness(255);

  // Shifted register
  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);

  // Ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inicjalizacja portu szeregowego
  Serial.begin(9600); 
  Serial.println("Serial initialized");
}

/*******************************************************************
 * Runtime functions definitions
 *******************************************************************/

/**
 * This function simply prints the welcome screen
 * on the segment display.
 * 
 * @author Piotr Kluch
 * @since 1.0
 */
void printWelcomeScreen() {
  delay(1000);
  disp.writeStr("    HI");
  delay(200);
  disp.writeStr("   HI");
  delay(200);
  disp.writeStr("  HI");
  delay(200);
  disp.writeStr(" HI");
  delay(200);
  disp.writeStr("HI");
  delay(3000);
  disp.writeStr(" HI");
  delay(100);
  disp.writeStr("  HI");
  delay(100);
  disp.writeStr("   HI");
  delay(100);
  disp.writeStr("    HI");
  delay(1000);
}

/**
 * This function measures the distance (free air height)
 * above the ultrasonic sensor.
 * 
 * @author Adam Gruszka
 * @since 1.0
 */
void updateDistance() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  // Ustawiamy trigPin w stan niski - rozpoczynamy pomiar
  digitalWrite(trigPin, LOW);
  // Odczytujamy czas trwania stanu wysokiego na pinie echoPin
  responseTime = pulseIn(echoPin, HIGH);
  // Obliczamy odległość
  distance = responseTime / 58;
}

void checkIsSnoozeButtonPressed() {
  updateDistance();
  Serial.println(distance);
  if ( previousDistance > 0 && abs(distance-previousDistance) > 50 ) {
    //Serial.println("BREAK");
    if (i<=0)
      i = 100;
    i += 500;
    snoozeButtonPressed = true;
    for (int x = 0; x<3; x++)
      beep(890, 100);
    // Uncomment lines below in order to enable extra 
    // timer pause between snoozes.
    //if (enabled)
    //  zatrzymajZegar();
  }
  previousDistance = distance;
  if ( distance < 3 ) {
    beep(200, 2000);
    resetFunc();
  }
}

void turnOffRGBDiode() {
  digitalWrite(STCP_pin, LOW);
  shiftOut(DS_pin, SHCP_pin, LSBFIRST, 0);
  // Return the latch pin high to signal chip that it 
  // no longer needs to listen for information
  digitalWrite(STCP_pin, HIGH);
}

/*******************************************************************
 * Loops
 *******************************************************************/

/**
 * This function is responsible for the alarm going off.
 *
 * In this function:
 * 1) Animate RGB Diode (through shifted register)
 * 2) Play piezo speaker
 * 3) Modulate vibration motor
 * Note: checkIsSnoozeButtonPressed() alters the alarm
 * and increases the alarm time by 5 additional seconds.
 * 
 * @author Piotr Kluch
 * @since 1.0
 */
void loop_Alarm() {
  while (snoozeButtonPressed==false) {
    float fixedRGBLoop = 512;
    float fixedBuzzer = 512;
    float fixedVibrationMotor = 1;
    for (int j = 0; j < 256; j++) {
      // Ground latchPin and hold low for as long as you are transmitting
      digitalWrite(STCP_pin, LOW);
      shiftOut(DS_pin, SHCP_pin, LSBFIRST, j);
      // Return the latch pin high to signal chip that it 
      // no longer needs to listen for information
      digitalWrite(STCP_pin, HIGH);
      // Run
      delay(fixedRGBLoop);
      fixedRGBLoop = fixedRGBLoop - fixedRGBLoop * 0.025;
      fixedBuzzer = fixedBuzzer - fixedBuzzer * 0.025;
      fixedVibrationMotor = fixedVibrationMotor + fixedBuzzer * 0.025;
      if (fixedBuzzer < 1.0e-6)
        fixedBuzzer = 512;
      analogWrite(buzzerPin, fixedBuzzer);
      analogWrite(vibrationMotorPin, fixedVibrationMotor);
      //Serial.println(j);
      checkIsSnoozeButtonPressed();
      if (snoozeButtonPressed) {
        analogWrite(buzzerPin, 0);
        analogWrite(vibrationMotorPin, 0);
        break;
      } 
    }
    if (!snoozeButtonPressed)
      starWarsTheme();
  }
  snoozeButtonPressed = false;
}

/**
 * Function responsible for displaying the time and the countdown.
 *
 * In this function:
 * 1) Animate RGB Diode (through shifted register)
 * 2) Play piezo speaker
 * 3) Modulate vibration motor
 * Note: checkIsSnoozeButtonPressed() alters the alarm
 * and increases the alarm time by 5 additional seconds.
 * 
 * @author Lukasz Ochmanski - Original implementation
 * @author Piotr Kluch - Modified to use other components.
 * @since 1.0
 */
void loop_Display() {
  disp.setBrightness(255);
  // It will reach 100 shortly after start
  //if (i < -100)
    //i=500;
  // While its still below 0
  if (i<=0) {
    // Welcome screen during first run
    if (programFirstRun) {
      //printWelcomeScreen();
    // Alarm screen when it hits T=0
    } else {
      disp.writeStr("CAFE");
      delay(50);
      for (int j = 1000; j>0; j--) {
        int a = j%50;
        if (a>0 && a<25)
          disp.setBrightness(255);
        else
          disp.setBrightness(0);
        delay(2);
      }
      loop_Alarm();
    }
  // When the timer is counting down
  } else {
    if (programFirstRun) {
      zatrzymajZegar();
      programFirstRun = false; 
    }
    int odczyt = digitalRead(buttonPin);
    if (odczyt != ostatni) {
      czasWcisniecia = millis();
    }
    if (minalMinimalnyOdstepCzasowy()) {
      if (nastapilaZmiana(odczyt) && jestPrzycisniety()) {
        zatrzymajZegar();
        beepSound();
      }
    }
    ostatni = odczyt;
    writeCyfra(i);
    disp.setDots(0,1,0,0);
    if ((i%100<5) && enabled)
      beep(880, 10);
    else
      delay(10);
    checkIsSnoozeButtonPressed();
  }
}

void loop_init() {
  loop_Display();
}

/**
 * Loops until any event happens (snooze button, restart).
 * Functionality and logic is implemented after (while) it runs.
 */
void loop() {
  // Uncomment lines below to test specific components
  //loop_Alarm();
  //loop_Display();

  turnOffRGBDiode();
  loop_init();
}

/*******************************************************************
 * Helper functions
 *******************************************************************/

// Custom ISR Timer routine
void timerIsr() {
  if (enabled) i--;
}
void writeCyfra(int val) {
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
    // Zauważyłem, że powinienem tutaj sprawdzać czy zero jest z lewej strony
    // wstawianie pustego znaku nie powinno następować z prawej strony.
    if (m_data[i]==0 && (i==0 || (i!=0 && m_data[i-1]==0))) {
      Wire.write(' ');
    }
    else {
      Wire.write(m_data[i]);
    }
  }
  Wire.endTransmission();
}
boolean jestPrzycisniety() {
  return obecny == HIGH;
}
boolean nastapilaZmiana(int odczyt) {
  boolean b = odczyt != obecny;
  if (b) { obecny = !obecny; }
  return b;
}
boolean minalMinimalnyOdstepCzasowy() {
  return (millis() - czasWcisniecia) > minimalnyCzasWcisniecia;
}
void zatrzymajZegar() {
  enabled = !enabled;
  digitalWrite(ledPin, enabled);
}

/*******************************************************************
 * Other / Miscellaneous
 *******************************************************************/

void beepSound() {
  beep(523, 250);
}

/* Begin StarWars theme buzzer */
// Below are two octaves, which are needed to play the melody.
// Initialize following notes: cdefgahc, CDEFGAHC, 
// and needed sharp notes (higher pitch)
const int c = 261, d = 294, e = 329,f = 349, g = 391;
const int gS = 415, a = 440, aS = 455, b = 466, cH = 523;
const int cSH = 554, dH = 587, dSH = 622, eH = 659, fH = 698;
const int fSH = 740, gH = 784, gSH = 830, aH = 880;
const int ledPin1 = 12, ledPin2 = 13;
int counter = 0;
void starWarsTheme() {
  delay(3000);
  //Play first section
  firstSection();
  //Play second section
  secondSection();
  //Variant 1
  beep(f, 250); beep(gS, 500); beep(f, 350);  
  beep(a, 125); beep(cH, 500); beep(a, 375); 
  beep(cH, 125); beep(eH, 650);
  delay(500);
  //Repeat second section
  secondSection();
  //Variant 2
  beep(f, 250); beep(gS, 500); beep(f, 375);  
  beep(cH, 125); beep(a, 500); beep(f, 375);  
  beep(cH, 125); beep(a, 650);  
  delay(650);
}

/**
 * Play a 'beep' on the piezo speaker.
 * 
 * @author Piotr Kluch
 * @since 1.0
 */
void beep(int note, int duration) {
  // Play tone on buzzerPin
  tone(buzzerPin, note, duration);
  // Play different LED depending on value of 'counter'
  if(counter % 2 == 0) {
    digitalWrite(ledPin1, HIGH);
    delay(duration);
    digitalWrite(ledPin1, LOW);
  } else {
    digitalWrite(ledPin1, HIGH);
    delay(duration);
    digitalWrite(ledPin1, LOW);
  }
  // Stop tone on buzzerPin
  noTone(buzzerPin);
  delay(50);
  // Increment counter
  counter++;
}
void firstSection() {
  beep(a, 500); beep(a, 500); beep(a, 500);
  beep(f, 350); beep(cH, 150); beep(a, 500);
  beep(f, 350); beep(cH, 150); beep(a, 650);
  delay(500);
  beep(eH, 500); beep(eH, 500); beep(eH, 500);  
  beep(fH, 350); beep(cH, 150); beep(gS, 500);
  beep(f, 350); beep(cH, 150); beep(a, 650);
  delay(500);
}
void secondSection() {
  beep(aH, 500); beep(a, 300); beep(a, 150);
  beep(aH, 500); beep(gSH, 325); beep(gH, 175);
  beep(fSH, 125); beep(fH, 125); beep(fSH, 250);
  delay(325);
  beep(aS, 250); beep(dSH, 500); beep(dH, 325);  
  beep(cSH, 175); beep(cH, 125); beep(b, 125);  
  beep(cH, 250);  
  delay(350);
}
/* End StarWars theme buzzer */
