#include <Arduino.h>

// Definisi Pin Sensor Filling
const int PIN_SENSOR_FILL = 6;
const int PIN_EN_FILL = 10;

// Definisi Pin Sensor Capping
const int PIN_SENSOR_CAP = 18;
const int PIN_EN_CAP = 17;

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_SENSOR_FILL, INPUT);
  pinMode(PIN_EN_FILL, OUTPUT);
  
  pinMode(PIN_SENSOR_CAP, INPUT);
  pinMode(PIN_EN_CAP, OUTPUT);
  
  Serial.println("=== Sistem Pengujian Interaktif ===");
  Serial.println("Ketik 'A' untuk cek Filling, 'B' untuk cek Capping");
}

void loop() {
  // Cek apakah ada data yang masuk dari Serial Monitor
  if (Serial.available() > 0) {
    char input = Serial.read(); // Membaca karakter yang diketik

    // --- Cek Filling (Ketik A) ---
    if (input == 'A' || input == 'a') {
      digitalWrite(PIN_EN_FILL, HIGH);
      delay(50);
      bool botolFilling = (digitalRead(PIN_SENSOR_FILL) == LOW);
      digitalWrite(PIN_EN_FILL, LOW);
      
      Serial.print("[TEST] Filling: ");
      Serial.println(botolFilling ? "ADA BOTOL" : "KOSONG");
    }

    // --- Cek Capping (Ketik B) ---
    else if (input == 'B' || input == 'b') {
      digitalWrite(PIN_EN_CAP, HIGH);
      delay(50);
      bool botolCapping = (digitalRead(PIN_SENSOR_CAP) == LOW);
      digitalWrite(PIN_EN_CAP, LOW);
      
      Serial.print("[TEST] Capping: ");
      Serial.println(botolCapping ? "ADA BOTOL" : "KOSONG");
    }
  }
}