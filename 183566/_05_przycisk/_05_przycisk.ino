const int buttonPin = 2;    // pin, który zczytuje wartości
const int ledPin = 13;      // dioda

int zapalona = HIGH;
int obecny;
int ostatni = LOW;
long czasWcisniecia = 0;
long minimalnyCzasWcisniecia = 50;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, zapalona);
}

void loop() {
  int odczyt = digitalRead(buttonPin);
  if (odczyt != ostatni) {
    czasWcisniecia = millis();
  } 
  
  if (minalMinimalnyOdstepCzasowy()) {
    if (odczyt != obecny) {
      obecny = !obecny;
      if (obecny == HIGH) {
        przelaczLampke(zapalona);
      }
    }
  }
  ostatni = odczyt;
}

boolean minalMinimalnyOdstepCzasowy() {
  return (millis() - czasWcisniecia) > minimalnyCzasWcisniecia;
}

void przelaczLampke(int stan) {
  zapalona = !stan;
  digitalWrite(ledPin, zapalona);
}

