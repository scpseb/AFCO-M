/*
AFCO-M Automatic Filling And Capping Oil Machine

| ESP32-S3
*/

#include <Arduino.h>
#include <Servo.h> //dlloydev/ESP32 ESP32S2 AnalogWrite@^5.0.2

// --- DEFINISI PIN ---
//Button
const int PIN_TOMBOL = 21;

// Rotary (Driver 2)
const int DIR_PIN_ROTARY = 8;
const int STEP_PIN_ROTARY = 9;

// Capping
const int PIN_IR_OUT_CAP = 18; // Sensor IR Stasiun Capping LLC LV3
const int PIN_IR_EN_CAP = 17;
const int PIN_DIR_CAP = 13;
const int PIN_STEP_CAP = 14;
const int PIN_SERVO_CAPPING = 15; // Channel 2

// Filling
const int PIN_SENSOR_AIR = 4;     // LLC LV4
const int PIN_SERVO_1 = 5;        // Channel 0 (Keran Penampungan Sementara)
const int PIN_IR_OUT_FILL = 6;    // Sensor IR Stasiun Filling LLC LV2
const int PIN_SERVO_2 = 7;        // Channel 1 (Keran Botol)
const int PIN_IR_EN_FILL = 10;

// --- KONFIGURASI MOTOR ---
  // ROTARY
const int STEPS_PER_STATION = 600; // Untuk microstepping 1/8 dan puli 1:3
const int DELAY_AWAL = 2200; 
const int DELAY_MIN = 500;   
  // CAPPING
int LANGKAH_TURUN_CAPPING = 1400;
int JEDA_KECEPATAN_CAP = 500;

// --- OBJEK SERVO ---
Servo servoCapping;
Servo servo1;
Servo servo2;

// --- VARIABEL SIKLUS BATCH ---
int hitunganSiklus = 0;

// --- FUNGSI Rotary ---
void RotarySpin(int jumlahLangkah) {
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

// --- FUNGSI PERSIAPAN TANGKI SEMENTARA ---
void persiapanTangkiSementara() {
  Serial.println("\n[PERSIAPAN] Menyiapkan Tangki Sementara...");
  
  // Buka keran tangki utama
  servo1.write(PIN_SERVO_1, 53); 
  Serial.println("-> Keran Tangki Utama DIBUKA. Menunggu air penuh...");
  
  // Tunggu sampai sensor air mendeteksi penuh (Asumsi HIGH = Penuh)
  while (digitalRead(PIN_SENSOR_AIR) == LOW) {
    delay(50);
  }
  
  Serial.println("-> Tangki Sementara PENUH!");
  
  // Tutup keran tangki utama
  servo1.write(PIN_SERVO_1, 90); 
  delay(1000);
}

// --- FUNGSI FILLING ---
void eksekusiFilling() {
  Serial.println("-> Mengecek Botol di Stasiun Filling...");
  digitalWrite(PIN_IR_EN_FILL, HIGH);
  delay(200); // Stabilisasi IR

  if (digitalRead(PIN_IR_OUT_FILL) == LOW) { // Asumsi LOW = terdeteksi
    Serial.println("-> Botol terdeteksi! Memulai Filling.");
    digitalWrite(PIN_IR_EN_FILL, LOW);
    
    // Buka keran pengisian botol
    servo2.write(PIN_SERVO_2, 0); 
    delay(6000); // Waktu pengisian 6 detik
    
    // Tutup keran pengisian botol
    servo2.write(PIN_SERVO_2, 90); 
    delay(500); // Tunggu servo menutup rapat
    
    Serial.println("-> Filling Selesai.");
  } else {
    Serial.println("-> Botol kosong, filling dilewati.");
    digitalWrite(PIN_IR_EN_FILL, LOW);
  }
}

// --- FUNGSI CAPPING ---
void eksekusiCapping() {
  Serial.println("-> Mengecek Botol di Stasiun Capping...");
  digitalWrite(PIN_IR_EN_CAP, HIGH);
  delay(200);

  if (digitalRead(PIN_IR_OUT_CAP) == LOW) {
    Serial.println("-> Botol terdeteksi! Memulai Capping.");
    digitalWrite(PIN_IR_EN_CAP, LOW);

    // Stepper Turun
    digitalWrite(PIN_DIR_CAP, HIGH);
    for(int i = 0; i < LANGKAH_TURUN_CAPPING; i++) {
      digitalWrite(PIN_STEP_CAP, HIGH); delayMicroseconds(JEDA_KECEPATAN_CAP);
      digitalWrite(PIN_STEP_CAP, LOW); delayMicroseconds(JEDA_KECEPATAN_CAP);
    }
    delay(500);
    
    // Servo Putar (Tutup Botol)
    servoCapping.write(PIN_SERVO_CAPPING, 180);
    delay(1500);

    // Stepper Naik
    digitalWrite(PIN_DIR_CAP, LOW);
    for(int i = 0; i < LANGKAH_TURUN_CAPPING; i++) {
      digitalWrite(PIN_STEP_CAP, HIGH); delayMicroseconds(JEDA_KECEPATAN_CAP);
      digitalWrite(PIN_STEP_CAP, LOW); delayMicroseconds(JEDA_KECEPATAN_CAP);
    }
    
    // Kembalikan Posisi Servo
    servoCapping.write(PIN_SERVO_CAPPING, 0);
    delay(1000);
    Serial.println("-> Capping Selesai.");
  } else {
    Serial.println("-> Botol kosong, capping dilewati.");
    digitalWrite(PIN_IR_EN_CAP, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Setup Pin Stepper Rotary & Stepper Capping
  pinMode(STEP_PIN_ROTARY, OUTPUT);
  pinMode(DIR_PIN_ROTARY, OUTPUT);
  pinMode(PIN_DIR_CAP, OUTPUT);
  pinMode(PIN_STEP_CAP, OUTPUT);
  
  // Setup Pin Sensor Capping
  pinMode(PIN_IR_OUT_CAP, INPUT);
  pinMode(PIN_IR_EN_CAP, OUTPUT);
  digitalWrite(PIN_IR_EN_CAP, LOW);
  
  // Setup Pin Sensor Filling
  pinMode(PIN_SENSOR_AIR, INPUT);
  pinMode(PIN_IR_OUT_FILL, INPUT);
  pinMode(PIN_IR_EN_FILL, OUTPUT);
  digitalWrite(PIN_IR_EN_FILL, LOW);

  // Inisialisasi Servo
  servo1.attach(PIN_SERVO_1, 0);
  servo2.attach(PIN_SERVO_2, 1);
  servoCapping.attach(PIN_SERVO_CAPPING, 2);
  
  // Posisi Awal Semua Servo Tertutup/Standby
  servo1.write(PIN_SERVO_1, 90);
  servo2.write(PIN_SERVO_2, 90);
  delay(500);
  servoCapping.write(PIN_SERVO_CAPPING, 0);

  Serial.println("=== SISTEM INTEGRASI SIAP ===");
}

void loop() {

  if (hitunganSiklus == 0) {
    Serial.println("\n[SISTEM STANDBY] Menekan tombol untuk memulai 1 Batch (8 Langkah)...");
    
    // Program akan terkunci di sini selama tombol belum ditekan (Pin membaca HIGH)
    while (digitalRead(PIN_TOMBOL) == HIGH) {
      delay(50);
    }
    
    Serial.println("-> TOMBOL DITEKAN! Memulai Proses Mesin...");
    delay(1000); // Debounce agar tombol tidak terbaca ganda
  }
  Serial.println("\n=================================");
  Serial.println("SIKLUS BATCH KE: ");
  Serial.println(hitunganSiklus +1);
  Serial.println("=================================");
  
  // 1. Pastikan tangki sementara penuh sebelum siklus putaran dimulai
  persiapanTangkiSementara();

  // 2. Putar meja 1 stasiun (45 derajat)
  Serial.println("\nMemutar Rotary Table...");
  RotarySpin(STEPS_PER_STATION);
  delay(1000); // Jeda sejenak agar botol stabil setelah berputar
  
  // 3. Inspeksi Stasiun (Cek Filling dan Capping bersamaan dalam satu siklus)
  Serial.println("--- Inspeksi Stasiun Aktif ---");
  
  // A. Lakukan proses Filling jika ada botol di stasiun pengisian
  eksekusiFilling();
  
  // B. Lakukan proses Capping jika ada botol di stasiun penutupan
  eksekusiCapping();
  
  // 4. Tambah nilai penghitung siklus setelah 1 kali meja bergerak
  hitunganSiklus++;

  // 5. Cek apakah sudah mencapai batas 8 kali jalan (1 Batch penuh)
  if (hitunganSiklus >= 8) {
    Serial.println("\n[INFO BATCH SELESAI] Rotary Table telah berputar 8 langkah.");
    Serial.println("Sistem dijeda otomatis. Menunggu konfirmasi tombol kembali...");
    
    // Kembalikan ke 0, sehingga loop() selanjutnya akan tertahan lagi di jebakan tombol
    hitunganSiklus = 0; 
  }
  
  Serial.println("Siklus selesai. Bersiap ke langkah selanjutnya...");
  delay(1000);
}