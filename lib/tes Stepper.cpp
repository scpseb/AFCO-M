#include <Arduino.h>

// Definisi Pin (Sesuaikan dengan wiring Anda)
const int STEP_PIN = 14; 
const int DIR_PIN = 13;

// --- Pengaturan Mode 1 (Full Step) ---
const int FULL_STEP_STEPS = 1600;
const int FULL_STEP_DELAY = 500; // Mikrodetik

// --- Pengaturan Mode 2 (1/8 Step) ---
const int MICRO_STEP_STEPS = 600;
const int DELAY_AWAL = 2000;
const int DELAY_MIN = 500;

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("--- Memulai Pengujian Motor Stepper ---");
}

// Fungsi Mode 1: Full Step Bolak-Balik
void testFullStep() {
  Serial.println("Mode: Full Step (1600 langkah, bolak-balik)");
  
  // Maju
  digitalWrite(DIR_PIN, HIGH);
  for(int i = 0; i < FULL_STEP_STEPS; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(FULL_STEP_DELAY);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(FULL_STEP_DELAY);
  }
  delay(1000);

  // Mundur
  digitalWrite(DIR_PIN, LOW);
  for(int i = 0; i < FULL_STEP_STEPS; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(FULL_STEP_DELAY);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(FULL_STEP_DELAY);
  }
  delay(1000);
}

// Fungsi Mode 2: 1/8 Step Ramping Satu Arah
void testMicroStep() {
  Serial.println("Mode: 1/8 Step (600 langkah, ramping satu arah)");
  digitalWrite(DIR_PIN, HIGH);
  
  int currentDelay = DELAY_AWAL;
  
  for(int i = 0; i < MICRO_STEP_STEPS; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(currentDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(currentDelay);
    
    // Logika Ramping (Percepat ke DELAY_MIN)
    if (currentDelay > DELAY_MIN) {
      currentDelay -= 5; // Ubah nilai ini untuk kecepatan akselerasi
    }
  }
  delay(2000);
}

void loop() {
  testFullStep();
  testMicroStep();
}