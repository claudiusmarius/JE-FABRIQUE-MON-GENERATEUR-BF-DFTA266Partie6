#include <SPI.h>
#include <Wire.h>
#include <MD_AD9833.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --------------------
// AD9833
// --------------------
#define SDATA 10
#define SCLK  8
#define FSYNC 9
MD_AD9833 AD(SDATA, SCLK, FSYNC);

// --------------------
// OLED 128x32
// --------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --------------------
// Entrées
// --------------------
#define POT_PIN   A0
#define MODE_POT  A1
#define BTN_UP    2
#define BTN_DOWN  3

// LEDs
#define LED_MODE0 6
#define LED_MODE1 7

// --------------------
// Plages
const long minF[6] = {1, 10, 100, 1000, 10000, 100000};
const long maxF[6] = {10, 100, 1000, 10000, 100000, 1000000};

// --------------------
int currentRange = -1;
long currentFreq = 0;
long offset = 0;

// pot fréquence
float filteredPot = 0;
const float alpha = 0.02;

// 🔥 SUPPRESSION filtre mode
int lastMode = -1;

// Sample & Hold
long lastPotRange = -1;
unsigned long lastPotChangeTime = 0;
const int POT_STABLE_TIME = 200;

// boutons
unsigned long lastPress = 0;
const unsigned long DEBOUNCE_MS = 30;
bool lastUpState = HIGH;
bool lastDownState = HIGH;

// noms des modes pour OLED
const char* modeNames[] = {"SINE", "TRIANGLE", "SQUARE"};

// --------------------
void setup() {
  delay(200);

  Wire.begin();
  Wire.end();
  delay(10);
  Wire.begin();
  delay(10);

  analogReadResolution(12);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(LED_MODE0, OUTPUT);
  pinMode(LED_MODE1, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) while(1);
  delay(50);

  display.clearDisplay();
  display.display();
  delay(50);

  AD.begin();
  AD.setMode(MD_AD9833::MODE_SINE);

  // 🔥 INITIALISATION
  filteredPot = analogRead(POT_PIN);
  int initRange = filteredPot / 683;
  if (initRange > 5) initRange = 5;
  if (initRange < 0) initRange = 0;

  currentRange = initRange;
  lastPotRange = initRange;

  offset = 0;
  currentFreq = minF[currentRange];
  AD.setFrequency(MD_AD9833::CHAN_0, currentFreq);

  // init mode
  int modeADC = analogRead(MODE_POT);
  if (modeADC < 1365) lastMode = 0;
  else if (modeADC < 2730) lastMode = 1;
  else lastMode = 2;

  switch (lastMode) {
    case 0: AD.setMode(MD_AD9833::MODE_SINE); break;
    case 1: AD.setMode(MD_AD9833::MODE_TRIANGLE); break;
    case 2: AD.setMode(MD_AD9833::MODE_SQUARE1); break;
  }

  displayFreq(currentFreq);
}

// --------------------
void loop() {
  if (currentRange < 0) return;

  // --- lecture pot fréquence ---
  int potADC = analogRead(POT_PIN);
  filteredPot = alpha * potADC + (1 - alpha) * filteredPot;

  int newRange = filteredPot / 683;
  if (newRange > 5) newRange = 5;

  if (newRange != lastPotRange) {
    lastPotChangeTime = millis();
    lastPotRange = newRange;
  }

  if (millis() - lastPotChangeTime > POT_STABLE_TIME) {
    if (newRange != currentRange) {
      currentRange = newRange;
      offset = 0;
    }
  }

  // --- mode ---
  int modeADC = analogRead(MODE_POT);
  int newMode = 0;
  if (modeADC < 1365) newMode = 0;
  else if (modeADC < 2730) newMode = 1;
  else newMode = 2;

  if (newMode != lastMode) {
    lastMode = newMode;

    switch (newMode) {
      case 0: AD.setMode(MD_AD9833::MODE_SINE); break;
      case 1: AD.setMode(MD_AD9833::MODE_TRIANGLE); break;
      case 2: AD.setMode(MD_AD9833::MODE_SQUARE1); break;
    }

    displayFreq(currentFreq);
  }

  // --- LEDs ---
  switch (lastMode) {
    case 0: digitalWrite(LED_MODE0, LOW); digitalWrite(LED_MODE1, LOW); break;
    case 1: digitalWrite(LED_MODE0, HIGH); digitalWrite(LED_MODE1, LOW); break;
    case 2: digitalWrite(LED_MODE0, LOW); digitalWrite(LED_MODE1, HIGH); break;
  }

  // --- boutons ---
  handleButtons();

  // --- calcul fréquence ---
  long rawFreq = minF[currentRange] + offset;
  long step = getStep(currentRange);

  long newFreq;
  if (currentRange == 0) newFreq = minF[0] + offset; // 1–10 Hz précis
  else newFreq = minF[currentRange] + ((offset / step) * step);

  if (newFreq < minF[currentRange]) newFreq = minF[currentRange];
  if (newFreq > maxF[currentRange]) newFreq = maxF[currentRange];

  long threshold = (currentRange == 0) ? 0 : 5;
  if (abs(newFreq - currentFreq) > threshold) {
    currentFreq = newFreq;
    AD.setFrequency(MD_AD9833::CHAN_0, currentFreq);
    displayFreq(currentFreq);
  }
}

// --------------------
long getStep(int range) {
  switch (range) {
    case 0: return 1;
    case 1: return 2;   // 10–100 Hz step initial
    case 2: return 5;   // 100–1000 Hz step initial
    case 3: return 5;
    case 4: return 20;
    case 5: return 100;
    default: return 1;
  }
}

// --------------------
bool readButton(int pin, bool &lastState) {
  bool state = digitalRead(pin);
  if (state != lastState) {
    delay(DEBOUNCE_MS);
    state = digitalRead(pin);
  }
  lastState = state;
  return state == LOW;
}

// --------------------
void handleButtons() {
  static unsigned long pressStart = 0;
  static bool btnHeld = false;

  bool up = readButton(BTN_UP, lastUpState);
  bool down = readButton(BTN_DOWN, lastDownState);
  unsigned long now = millis();

  if (up || down) {
    if (!btnHeld) {
      pressStart = now;
      btnHeld = true;
    }

    unsigned long heldTime = now - pressStart;
    long step;
    unsigned long tempo;

    // --- logique par plage ---
    switch (currentRange) {
      case 0: // 1–10Hz
        step = 1;
        tempo = 150;
        break;
      case 1: // 10–100Hz
        step = 2 + heldTime / 300; // step progressif
        if (step > 10) step = 10;
        tempo = 80;
        break;
      case 2: // 100–1000Hz
        step = 5 + heldTime / 200; // step progressif
        if (step > 50) step = 50;
        tempo = 70;
        break;
      default: // >1 kHz
        step = getStep(currentRange) * (1 + heldTime / 200);
        long maxStep = (maxF[currentRange] - minF[currentRange]) / 10;
        if (step > maxStep) step = maxStep;
        tempo = 50;
        break;
    }

    if (now - lastPress > tempo) {
      if (up) offset += step;
      if (down) offset -= step;

      if (offset < 0) offset = 0;
      long maxOffset = maxF[currentRange] - minF[currentRange];
      if (offset > maxOffset) offset = maxOffset;

      lastPress = now;
    }
  } else {
    btnHeld = false;
  }
}

// --------------------
void displayFreq(long freq) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  char buffer[16];
  if (freq < 1000) sprintf(buffer, "%ld Hz", freq);
  else if (freq < 10000) sprintf(buffer, "%.2f kHz", freq / 1000.0);
  else if (freq < 100000) sprintf(buffer, "%.1f kHz", freq / 1000.0);
  else sprintf(buffer, "%ld kHz", freq / 1000);

  display.setTextSize(1);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  int cursorX = (SCREEN_WIDTH - w) / 2;
  int cursorY = (SCREEN_HEIGHT - h) / 2 - 4;
  display.setCursor(cursorX, cursorY);
  display.print(buffer);

  const char* modeStr = modeNames[lastMode];
  display.getTextBounds(modeStr, 0, 0, &x1, &y1, &w, &h);
  cursorX = (SCREEN_WIDTH - w) / 2;
  cursorY += h + 2;
  display.setCursor(cursorX, cursorY);
  display.print(modeStr);

  display.display();
}