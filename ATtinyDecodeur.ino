#define IN_A 3
#define IN_B 4

#define LED_SINE     0
#define LED_TRIANGLE 1
#define LED_SQUARE   2

int lastMode = -1;

int getMode() {
  int A = digitalRead(IN_A);
  int B = digitalRead(IN_B);

  if (A == 0 && B == 0) return 0; // SINE
  if (A == 1 && B == 0) return 1; // TRIANGLE
  return 2;                       // SQUARE
}

void setLEDs(int mode) {
  digitalWrite(LED_SINE,     mode == 0);
  digitalWrite(LED_TRIANGLE, mode == 1);
  digitalWrite(LED_SQUARE,   mode == 2);
}

void blinkAll(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_SINE, HIGH);
    digitalWrite(LED_TRIANGLE, HIGH);
    digitalWrite(LED_SQUARE, HIGH);
    delay(80);

    digitalWrite(LED_SINE, LOW);
    digitalWrite(LED_TRIANGLE, LOW);
    digitalWrite(LED_SQUARE, LOW);
    delay(80);
  }
}

void setup() {
  pinMode(IN_A, INPUT);
  pinMode(IN_B, INPUT);

  pinMode(LED_SINE, OUTPUT);
  pinMode(LED_TRIANGLE, OUTPUT);
  pinMode(LED_SQUARE, OUTPUT);

  delay(100); // stabilité au démarrage
}

void loop() {
  int mode = getMode();

  if (mode != lastMode) {
    blinkAll(2);   // animation
    setLEDs(mode); // affichage final
    lastMode = mode;
  }
}