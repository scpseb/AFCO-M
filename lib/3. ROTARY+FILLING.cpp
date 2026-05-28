#include <Arduino.h>
#include <Servo.h>

// --- DEFINISI PIN ---
const int WATER_SENSOR_PIN = 4; // [cite: 58]
const int SERVO_1_PIN = 5;      // [cite: 58]
const int IR_SENSOR_PIN = 6;    // [cite: 58]
const int SERVO_2_PIN = 7;      // [cite: 59]
const int DIR_PIN = 8;          // [cite: 59]
const int STEP_PIN = 9;         // [cite: 59]
const int EN_IR_PIN = 10;       // Pin baru untuk Enable Sensor IR

// --- KONFIGURASI STEPPER ---
const int STEPS_PER_STATION = 200; // Asumsi Microstepping 1/8
const int DELAY_SPEED = 1500;
int currentStation = 0;

// --- KONFIGURASI SERVO ---
Servo servo1;
Servo servo2;
const int POS_BUKA = 0;   
const int POS_TUTUP = 90; 

void gerakStepperSearahJarumJam(int totalLangkah) {
  digitalWrite(DIR_PIN, LOW); 
  
  for(int i = 0; i < totalLangkah; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(DELAY_SPEED);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(DELAY_SPEED);
  }
}

void setup() {
  Serial.begin(115200);
  delay(8000); 

  // Inisialisasi Pin Sensor dan Stepper
  pinMode(WATER_SENSOR_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(EN_IR_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  // Pastikan sensor IR dalam keadaan MATI di awal
  digitalWrite(EN_IR_PIN, LOW);

  // Inisialisasi Pin Servo
  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);

  // Posisi Awal 
  servo1.write(POS_BUKA); 
  servo2.write(POS_BUKA); 
  
  Serial.println("=== Mesin Dinyalakan ===");
}

void loop() {
  Serial.println("\n[SIKLUS DIMULAI]");
  
  // 1. Servo 1 Membuka & Menunggu Air
  Serial.println("Servo 1 Membuka. Menunggu air...");
  servo1.write(POS_BUKA);

  while (digitalRead(WATER_SENSOR_PIN) == LOW) {
    delay(50); 
  }

  // 2. Air Terdeteksi, Servo 1 Bergerak Tutup
  Serial.println("Air TERDETEKSI! Servo 1 menutup (90 derajat)...");
  servo1.write(POS_TUTUP);
  delay(3000); 

  // 3. Rotary Table Memutar 1 Step (45 Derajat)
  if (currentStation < 6) {
    Serial.print("Memutar 45 Derajat ke Stasiun: ");
    Serial.println(currentStation + 1);
    
    gerakStepperSearahJarumJam(STEPS_PER_STATION);
    currentStation++; 
  } else {
    Serial.println("Mencapai 6 Langkah! Mereset ke 0 Derajat searah jarum jam...");
    int sisaLangkahKeNol = STEPS_PER_STATION * 2; 
    gerakStepperSearahJarumJam(sisaLangkahKeNol);
    currentStation = 0; 
  }
  delay(3000); // Jeda stabilisasi rotary table 

  // 4. Proses Enable dan Deteksi Sensor IR
  Serial.println("Menyalakan Sensor IR (Enable)...");
  digitalWrite(EN_IR_PIN, HIGH); // Nyalakan sensor
  delay(200); // Jeda 200ms agar komponen di dalam sensor IR stabil sebelum membaca
  
  Serial.println("Sensor IR Siaga: Mengecek keberadaan botol...");
  
  // KY-032 mengirimkan sinyal LOW saat mendeteksi objek [cite: 58]
  if (digitalRead(IR_SENSOR_PIN) == LOW) { 
    Serial.println("Botol Terdeteksi! Servo 2 bergerak (Memulai Filling)...");
    
    servo2.write(POS_TUTUP); 
    delay(5000); // Waktu filling 5 detik
    
    Serial.println("Filling Selesai 5 Detik. Servo 2 kembali ke posisi awal.");
    servo2.write(POS_BUKA);
    delay(3000); 
    
  } else {
    Serial.println("Tidak ada botol terdeteksi. Melewati proses filling stasiun ini.");
  }

  // 5. Matikan Kembali Sensor IR
  Serial.println("Mematikan Sensor IR (Disable).");
  digitalWrite(EN_IR_PIN, LOW); // Matikan sensor untuk menghemat daya & cegah false trigger

  Serial.println("[SIKLUS SELESAI] Mengulang kembali...");
  Serial.println("---------------------------------------------------");
}