/*
PENGUJIAN KHUSUS SUBSISTEM ROTARY TABLE
PlatformIO Version | ESP32-S3
Spesifikasi: DRV8825 | Microstepping 1/8 | Rasio Puli 1:3 | 45 Derajat per Stasiun
*/

#include <Arduino.h>

// --- DEFINISI PIN ---
const int DIR_PIN_ROTARY = 8;
const int STEP_PIN_ROTARY = 9;

// --- KONFIGURASI MOTOR ---
// 200 langkah * 8 (microstep) * 3 (rasio puli) / 8 (stasiun) = 600 langkah per 45 derajat
const int STEPS_PER_STATION = 600; 
const int DELAY_AWAL = 2000; // Delay paling lambat saat awal berputar (mikrodetik)
const int DELAY_MIN = 500;   // Delay paling cepat saat putaran stabil (mikrodetik)

// --- FUNGSI GERAK HALUS MANUAL ---
void gerakRotaryHalus(int jumlahLangkah) {
  Serial.println("[ROTARY] Memulai putaran 45 derajat...");

  // Atur arah putaran (Ubah ke LOW jika arah putarannya terbalik)
  digitalWrite(DIR_PIN_ROTARY, HIGH);

  int currentDelay = DELAY_AWAL;
  int accelSteps = jumlahLangkah / 4; // 25% langkah awal/akhir untuk akselerasi/deselerasi

  for (int i = 0; i < jumlahLangkah; i++) {
    digitalWrite(STEP_PIN_ROTARY, HIGH);
    delayMicroseconds(currentDelay);
    digitalWrite(STEP_PIN_ROTARY, LOW);
    delayMicroseconds(currentDelay);
    
    // Akselerasi (Makin Cepat)
    if (i < accelSteps && currentDelay > DELAY_MIN) {
      currentDelay -= 5;
    }
    // Deselerasi (Makin Lambat)
    else if (i > (jumlahLangkah - accelSteps) && currentDelay < DELAY_AWAL) {
      currentDelay += 5;
    }
  }
  
  Serial.println("[ROTARY] Putaran selesai. Posisi terkunci.");
}

void setup() {
  Serial.begin(115200);
  delay(2000); // Jeda agar Native USB ESP32-S3 siap
  
  Serial.println("=== SISTEM ROTARY TABLE SIAP ===");

  // Inisialisasi Pin
  pinMode(STEP_PIN_ROTARY, OUTPUT);
  pinMode(DIR_PIN_ROTARY, OUTPUT);
  digitalWrite(STEP_PIN_ROTARY, LOW);
}

void loop() {
  // Putar meja sebesar 45 derajat (1 Stasiun)
  gerakRotaryHalus(STEPS_PER_STATION);
  
  // Jeda 3 detik untuk mensimulasikan proses filling/capping
  Serial.println("[SISTEM] Jeda simulasi 3 detik...\n");
  delay(3000); 
}