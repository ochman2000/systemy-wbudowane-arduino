/*
 * Ten program odlicza 10 sekund i wyświetla tylko te cyfry, które nie są zerem.
 *
 * 2014-12-20, Warszawa, Polska
 * Systemy Wbudowane Politechnika Łódzka
 * Łukasz Ochmański 183566
 */
 
 #ifndef TIMERONE_h
#define TIMERONE_h

#include <avr/io.h>
#include <avr/interrupt.h>

#define RESOLUTION 65536    // Timer1 is 16 bit
//  #include <TimerOne.h>
  #include <Wire.h>
  #include <TWIDisplay.h>
  
  #define SLAVE_ADDR 0x12
  
  void writeCyfra(int val);
  void initialize(long microseconds=1000000);
  void attachInterrupt(void (*isr)(), long microseconds=-1);
    void setPeriod(long microseconds);
        void (*isrCallback)();
        void resume();
    
  TWIDisplay disp(SLAVE_ADDR);
  
  const int buttonPin = 2;    // pin, który zczytuje wartości
  const int ledPin = 13;      // dioda
      unsigned char clockSelectBits;
	char oldSREG;					// To hold Status Register while ints disabled
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
      initialize(10000);
      attachInterrupt( timerIsr ); // attach the service routine here
  
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

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  isrCallback();
}


void initialize(long microseconds)
{
  TCCR1A = 0;                 // clear control register A 
  TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
  setPeriod(microseconds);
}

void attachInterrupt(void (*isr)(), long microseconds)
{
  if(microseconds > 0) setPeriod(microseconds);
  isrCallback = isr;                                       // register the user's callback with the real ISR
  TIMSK1 = _BV(TOIE1);                                     // sets the timer overflow interrupt enable bit
	// might be running with interrupts disabled (eg inside an ISR), so don't touch the global state
//  sei();
  resume();												
}

void resume()				// AR suggested
{ 
  TCCR1B |= clockSelectBits;
}

void setPeriod(long microseconds)		// AR modified for atomic access
{
  
  long cycles = (F_CPU / 2 000 000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum
  
  oldSREG = SREG;				
  cli();							// Disable interrupts for 16 bit register access
//  ICR1 = pwmPeriod = cycles;                                          // ICR1 is TOP in p & f correct pwm mode
  SREG = oldSREG;
  
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= clockSelectBits;                                          // reset clock select register, and starts the clock
}
#endif
