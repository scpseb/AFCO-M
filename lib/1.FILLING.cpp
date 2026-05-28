#include <Arduino.h>
#include <Servo.h>

/*
SUBSISTEM FILLING
*/

const int PIN_SENSOR_AIR = 4;
const int PIN_SERVO_1 = 5;
const int PIN_SENSOR_IR = 6;
const int PIN_SERVO_2 = 7;
const int PIN_EN_IR = 10;



// Membuat objek servo
Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200);
  delay(2000); // Jeda untuk inisialisasi Native USB ESP32-S3
  
  // Setup Sensor
  pinMode(PIN_SENSOR_AIR, INPUT);
  pinMode(PIN_SENSOR_IR, INPUT);
  pinMode(PIN_EN_IR, OUTPUT);
  
  // Matikan sensor IR di awal untuk menghemat daya dan cegah false trigger 
  digitalWrite(PIN_EN_IR, LOW);
  
  
  // Attach pin servo (Pin, min pulse, max pulse)
  servo1.attach(PIN_SERVO_1, 500, 2400); 
  servo2.attach(PIN_SERVO_2, 500, 2400);
  
  // Asumsi Posisi Awal: 
  // Servo 1 (90 derajat) = Tertutup, menahan air dari tanki utama ke penampungan sementara
  // Servo 2 (90 derajat) = Tertutup, menahan air jatuh ke botol
  servo1.write(PIN_SERVO_1,90);
  servo2.write(PIN_SERVO_2,90);
  
  Serial.println("Sistem Siap. Menunggu penampungan sementara penuh...");
}

void loop() {
  // ---------------------------------------------------------
  // TAHAP 1: Menunggu Air Penuh di Penampungan Sementara
  // ---------------------------------------------------------
  servo1.write(PIN_SERVO_1,0);
  Serial.println("\n[1] Menunggu air terdeteksi...");
  
  // Asumsi kabel MODE sensor air tidak dihubungkan (GND), output HIGH saat ada air [cite: 23]
  while (digitalRead(PIN_SENSOR_AIR) == LOW) {
    delay(100);
  }
  
  Serial.println("-> Air PENUH! Servo 1 memutar 90 derajat (Menutup keran utama).");
  servo1.write(PIN_SERVO_1,90);
  delay(1000); // Beri waktu servo 1 selesai bergerak

  // ---------------------------------------------------------
  // TAHAP 2: Mengaktifkan IR & Menunggu Botol
  // ---------------------------------------------------------
  Serial.println("[2] Mengaktifkan Sensor IR dan menunggu botol...");
  digitalWrite(PIN_EN_IR, HIGH); // Nyalakan sensor IR [cite: 228]
  delay(200); // Jeda stabilisasi agar ESP32 tidak membaca data "sampah" 

  // Sensor IR KY-032 secara default mengirimkan sinyal LOW saat mendeteksi objek [cite: 140]
  while (digitalRead(PIN_SENSOR_IR) == HIGH) {
    delay(100);
  }
  
  Serial.println("-> Botol terdeteksi! Servo 2 memutar 90 derajat (Membuka keran ke botol).");
  delay(500); // Jeda sejenak sebelum air mulai dituang
  servo2.write(PIN_SERVO_2,0);
  
  // ---------------------------------------------------------
  // TAHAP 3: Proses Filling (5 Detik)
  // ---------------------------------------------------------
  Serial.println("[3] Proses pengisian selama 5 detik...");
  delay(5000);
  
  Serial.println("-> Pengisian selesai. Memutar balik Servo 2 ke 90 derajat (Menutup).");
  servo2.write(PIN_SERVO_2,90);
  delay(1000); // Tunggu servo 2 tertutup rapat
  
  // Matikan IR kembali karena proses filling botol sudah selesai
  digitalWrite(PIN_EN_IR, LOW); 
  Serial.println("-> Sensor IR dimatikan.");

  // ---------------------------------------------------------
  // TAHAP 4: Membuka Keran Utama Kembali
  // ---------------------------------------------------------
  Serial.println("[4] Memutar balik Servo 1 ke 0 derajat (Membuka keran utama).");
  servo1.write(PIN_SERVO_1,0);
  delay(1000); // Tunggu servo 1 terbuka sempurna
  
  Serial.println("-> Siklus selesai. Mengisi ulang penampungan sementara...");
  delay(1000); 
}