/*
INTEGRASI SISTEM BATCH PROCESSING (ROTARY MANUAL RAMPING + CAPPING)
PlatformIO Version | ESP32-S3
*/

#include <Arduino.h>
#include <Servo.h>

// --- DEFINISI PIN ---
const int DIR_PIN_ROTARY = 8;
const int STEP_PIN_ROTARY = 9;
const int PIN_IR_OUT = 18;
const int PIN_IR_EN = 17;
const int PIN_DIR_CAP = 13;
const int PIN_STEP_CAP = 14;
const int PIN_SERVO_CAPPING = 15;

// --- KONFIGURASI MOTOR ---
const int STEPS_PER_STATION = 600;
const int DELAY_AWAL = 2000; 
const int DELAY_MIN = 500;   

Servo servoCapping;
int LANGKAH_TURUN_CAPPING = 1600;
int JEDA_KECEPATAN_CAP = 500;

// --- FUNGSI GERAK HALUS MANUAL (Pengganti AccelStepper) ---
void gerakRotaryHalus(int jumlahLangkah) {
  digitalWrite(DIR_PIN_ROTARY, HIGH);
  int currentDelay = DELAY_AWAL;
  int accelSteps = jumlahLangkah / 4; 

  for (int i = 0; i < jumlahLangkah; i++) {
    digitalWrite(STEP_PIN_ROTARY, HIGH);
    delayMicroseconds(currentDelay);
    digitalWrite(STEP_PIN_ROTARY, LOW);
    delayMicroseconds(currentDelay);
    
    // Akselerasi
    if (i < accelSteps && currentDelay > DELAY_MIN) currentDelay -= 5;
    // Deselerasi
    else if (i > (jumlahLangkah - accelSteps) && currentDelay < DELAY_AWAL) currentDelay += 5;
  }
}

// --- FUNGSI CAPPING ---
void eksekusiCapping() {
  Serial.println("-> Mengecek Botol di Stasiun Capping...");
  digitalWrite(PIN_IR_EN, HIGH);
  delay(200);

  if (digitalRead(PIN_IR_OUT) == LOW) {
    Serial.println("-> Botol terdeteksi! Memulai Capping.");
    digitalWrite(PIN_IR_EN, LOW);

    // Stepper Turun
    digitalWrite(PIN_DIR_CAP, LOW);
    for(int i = 0; i < LANGKAH_TURUN_CAPPING; i++) {
      digitalWrite(PIN_STEP_CAP, HIGH); delayMicroseconds(JEDA_KECEPATAN_CAP);
      digitalWrite(PIN_STEP_CAP, LOW); delayMicroseconds(JEDA_KECEPATAN_CAP);
    }
    delay(500);
    
    servoCapping.write(PIN_SERVO_CAPPING,180);
    delay(1500);

    // Stepper Naik
    digitalWrite(PIN_DIR_CAP, HIGH);
    for(int i = 0; i < LANGKAH_TURUN_CAPPING; i++) {
      digitalWrite(PIN_STEP_CAP, HIGH); delayMicroseconds(JEDA_KECEPATAN_CAP);
      digitalWrite(PIN_STEP_CAP, LOW); delayMicroseconds(JEDA_KECEPATAN_CAP);
    }
    
    servoCapping.write(PIN_SERVO_CAPPING,0);
    delay(1000);
  } else {
    Serial.println("-> Botol kosong, capping dilewati.");
    digitalWrite(PIN_IR_EN, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  pinMode(STEP_PIN_ROTARY, OUTPUT);
  pinMode(DIR_PIN_ROTARY, OUTPUT);
  pinMode(PIN_IR_OUT, INPUT);
  pinMode(PIN_IR_EN, OUTPUT);
  pinMode(PIN_DIR_CAP, OUTPUT);
  pinMode(PIN_STEP_CAP, OUTPUT);
  digitalWrite(PIN_IR_EN, LOW);

  // Inisialisasi Servo (Gunakan library ServoESP32 jika tersedia)
  servoCapping.attach(PIN_SERVO_CAPPING);
  servoCapping.write(PIN_SERVO_CAPPING,0);

  Serial.println("=== SISTEM INTEGRASI SIAP ===");
}

void loop() {
  Serial.println("Mulai Siklus Batch...");

  // FASE A: FILLING (4 Langkah)
  for (int i = 1; i <= 4; i++) {
    Serial.print("Rotary ke Stasiun Filling: "); Serial.println(i);
    gerakRotaryHalus(STEPS_PER_STATION);
    delay(3000); // Simulasi Filling
  }

  // FASE TRANSISI
  Serial.println("Transisi ke Capping...");
  gerakRotaryHalus(STEPS_PER_STATION);
  delay(1000);

  // FASE B: CAPPING
  eksekusiCapping();
  for (int i = 2; i <= 4; i++) {
    gerakRotaryHalus(STEPS_PER_STATION);
    eksekusiCapping();
  }

  Serial.println("Siklus Batch Selesai.");
  delay(5000);
}