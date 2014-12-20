/*
 * Ten program odlicza 10 sekund i wyświetla tylko te cyfry, które nie są zerem.
 *
 * 2014-12-20, Warszawa, Polska
 * Systemy Wbudowane Politechnika Łódzka
 * Łukasz Ochmański 183566
 */
#include <TimerOne.h>
#include <Wire.h>
#include <TWIDisplay.h>

#define SLAVE_ADDR 0x12

void writeCyfra(int val);
TWIDisplay disp(SLAVE_ADDR);

const int buttonPin = 2;    // pin, który zczytuje wartości
const int ledPin = 13;      // dioda

int enabled = HIGH;
int obecny;
int ostatni = LOW;
long czasWcisniecia = 0;
long minimalnyCzasWcisniecia = 50;
volatile int i;

void setup() {
    pinMode(buttonPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, enabled);

    // set a timer of length 10000 microseconds (or 0.01 sec - or 100Hz)
    Timer1.initialize(10000);
    Timer1.attachInterrupt( timerIsr ); // attach the service routine here

    Wire.begin();
    Serial.begin(9600);
    Serial.println("Serial Display Test");

    disp.clear();
    disp.setBrightness(255);
}

void loop()
{
    disp.setBrightness(255);
    if (i < -100) { i=1000; }
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

        disp.writeStr("SIUP");

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

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
    if (enabled) i--;
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
    if (b) {        obecny = !obecny;    }
    return b;
}

boolean minalMinimalnyOdstepCzasowy() {
    return (millis() - czasWcisniecia) > minimalnyCzasWcisniecia;
}

void zatrzymajZegar() {
    enabled = !enabled;
    digitalWrite(ledPin, enabled);

}
