/* Timer4000 - Final Program systemy-wbudowane-arduino
 *
 * 2014-12-20, Warszawa, Polska
 * Systemy Wbudowane Politechnika Łódzka
 * Piotr Kluch, 165436
 * Lukasz Ochmanski, 183566
 * Adam Gruszka, 165409
 */

// Libraries for display
#include <TimerOne.h>
#include <Wire.h>
#include <TWIDisplay.h>

// Definitions / declarations for display
#define SLAVE_ADDR 0x12
TWIDisplay disp(SLAVE_ADDR);
void writeCyfra(int val);

// Settings for display
const int buttonPin = 8;    // pin, który zczytuje wartości
const int ledPin = 13;      // dioda
int   enabled = HIGH;
int   obecny;
int   ostatni = LOW;
long  czasWcisniecia = 0;
long  minimalnyCzasWcisniecia = 50;
volatile int i;

// Settings RGB diodes
int   Trig = 2;  // Numer pinu wyzwolenia
int   Echo = 3;  // Numer pinu odpowiedzi
long  EchoTime;  // Czas trwania sygnału ECHO
int   Distance;  // Odległość w centymetrach
int   MaximumRange = 200; // Maksymalna odległość
int   MinimumRange = 2;   // Minimalna odległość

//RGB
int DS_pin = 8;
int STCP_pin = 9;
int SHCP_pin = 10;
boolean registers[8] = {LOW};
void writeReg() {
  digitalWrite(SHCP_pin, LOW);
  for (int i=7; i<=0; i--) {
    digitalWrite(STCP_pin, LOW);
    digitalWrite(DS_pin, registers[i]);
    digitalWrite(STCP_pin, HIGH);
  }
  digitalWrite(SHCP_pin, HIGH);
}

void setup() {
  //Settings Display
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, enabled);

  // set a timer of length 10000 microseconds (or 0.01 sec - or 100Hz)
  Timer1.initialize(10000);
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here

  Wire.begin();
  Serial.begin(9600); // Inicjalizacja portu szeregowego
  Serial.println("Serial Display Test");

  disp.clear();
  disp.setBrightness(255);

  //RGB
  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);
  writeReg();

  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
}

//RGB Diode
static int fixed = 500;

void loop_RGBDiode() {

  for (int j = 0; j < 256; j++) {
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(STCP_pin, LOW);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, j);   
    //return the latch pin high to signal chip that it 
    //no longer needs to listen for information
    digitalWrite(STCP_pin, HIGH);
    delay(fixed);
    fixed = fixed - fixed * 0.025;
    if (fixed < 0.00000000000000001)
    fixed = 500;
    Serial.println(j);
  }

}

/*
void loop_RGBDiode() {
  //shiftOut(5, 4, LSBFIRST, data);
  // Ustawiamy TRIG w stan niski na 2us
  //digitalWrite(Trig, LOW);
  //delayMicroseconds(2);
  // Ustawiamy TRIG w stan wysoki na 10us
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  // Ustawiamy TRIG w stan niski - rozpoczynamy pomiar
  digitalWrite(Trig, LOW);
  // Odczytujamy czas trwania stanu wysokiego na pinie ECHO
  EchoTime = pulseIn(Echo, HIGH);
  // Obliczamy odległość
  Distance = EchoTime / 58;

  // Sprawdzamy zakres pomiarowy
  if (Distance >= MaximumRange || Distance <= MinimumRange) {
    Serial.println("Poza zakresem"); 
    shiftOut(5, 4, LSBFIRST, data);
    data = 0;
  } else {
    // Jeśli powyżej 10cm zapalamy diodę zieloną
    // Jeśli poniżej 11cm zapalamy diodę czerwoną
    if (Distance < 11)
    {
      data = 9;
      // digitalWrite(Red, HIGH);      
      // digitalWrite(Green, LOW);
      shiftOut(5, 4, LSBFIRST, data);
    } else {
      data = 18;
      shiftOut(5, 4, LSBFIRST, data);
      // digitalWrite(Red, LOW);      
      // digitalWrite(Green, HIGH);
    }    
  }

  Serial.print("Odleglosc: ");
  Serial.println(Distance);
  Serial.print("Data: ");
  Serial.println(data);
  //data++;
  //if(data>=64)
  //{
  // data=0;
  //}
  delay(10);
}
*/

void loop_Display() {
  disp.setBrightness(255);
  if (i < -100) { i=3000; }
  if (i>0) {
    int odczyt = digitalRead(buttonPin);
    if (odczyt != ostatni) {
      czasWcisniecia = millis();
    }
    if (minalMinimalnyOdstepCzasowy()) {
      if (nastapilaZmiana(odczyt) && jestPrzycisniety()) {
        zatrzymajZegar();
      }
    }
    ostatni = odczyt;
    writeCyfra(i);
    disp.setDots(0,1,0,0);
    delay(10);
  } else {
    disp.writeStr("HAHA");
    delay(50);
    for (int j = 1000; j>0; j--) {
      int a = j%50;
      if (a>0 && a<25)
        disp.setBrightness(255);
      else
        disp.setBrightness(0);
      delay(2);
    }
  }
}

void loop() {
  //loop_RGBDiode();
  loop_Display();
}

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
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
    //zauważyłem, że powinienem tutaj sprawdzać czy zero jest z lewej strony
    //wstawianie pustego znaku nie powinno następować z prawej strony.
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