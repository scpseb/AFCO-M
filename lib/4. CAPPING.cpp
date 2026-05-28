#include <Arduino.h>
#include <Servo.h>

// Definisi Pin Subsistem Capping
const int PIN_IR_OUT = 18;
const int PIN_IR_EN = 17;
const int PIN_DIR = 13;
const int PIN_STEP = 14;
const int PIN_SERVO_CAPPING = 15; // Pin baru untuk Servo Capping

// Objek Servo
Servo servoCapping;

// Variabel Trial & Error Lead Screw
int LANGKAH_TURUN_CAPPING = 1600; // Silakan sesuaikan kembali angkanya
int JEDA_KECEPATAN = 500;        // Sesuaikan dengan konfigurasi microstepping Anda

void setup() {
  Serial.begin(115200);
  delay(2000); // Waktu inisialisasi Native USB

  // Inisialisasi Pin Stepper & Sensor
  pinMode(PIN_IR_OUT, INPUT);
  pinMode(PIN_IR_EN, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_STEP, OUTPUT);

  // Pastikan sensor IR dalam keadaan mati saat awal
  digitalWrite(PIN_IR_EN, LOW);

  // Attach pin servo (Pin, min pulse, max pulse)
  servoCapping.attach(PIN_SERVO_CAPPING, 500, 2400); 
  
  // Set posisi servo ke 0 derajat (Standby) saat mesin baru menyala
  servoCapping.write(PIN_SERVO_CAPPING,0);

  Serial.println("=========================================");
  Serial.println("Uji Coba Integrasi Capping (Stepper+Servo)");
  Serial.println("=========================================");
  Serial.println("Sistem Siap. Menunggu botol...");
}

void turunkanCapping(int langkah) {
  digitalWrite(PIN_DIR, HIGH); // Atur arah TURUN
  for(int i = 0; i < langkah; i++) {
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(JEDA_KECEPATAN);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(JEDA_KECEPATAN);
  }
}

void naikkanCapping(int langkah) {
  digitalWrite(PIN_DIR, LOW); // Atur arah NAIK
  for(int i = 0; i < langkah; i++) {
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(JEDA_KECEPATAN);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(JEDA_KECEPATAN);
  }
}

void loop() {
  Serial.println("\n[1] Menghidupkan Sensor IR & Mengecek Botol...");
  
  // Nyalakan sensor IR dan beri waktu stabilisasi
  digitalWrite(PIN_IR_EN, HIGH);
  delay(200); 

  // Program tertahan selama tidak ada botol (Sinyal HIGH)
  while (digitalRead(PIN_IR_OUT) == HIGH) {
    delay(100);
  }

  // Jika botol terdeteksi (Sinyal LOW)
  Serial.println("-> Botol terdeteksi! Memulai siklus capping.");
  
  // Matikan sensor IR agar aman dari getaran mesin
  digitalWrite(PIN_IR_EN, LOW);

  // --- URUTAN KERJA CAPPING ---

  Serial.println("[2] Stepper: Menurunkan mekanisme capping...");
  turunkanCapping(LANGKAH_TURUN_CAPPING);
  delay(500); // Jeda sejenak untuk menstabilkan tekanan lead screw ke botol
  
  Serial.println("[3] Servo: Memutar tutup botol 180 derajat (Mengencangkan)...");
  servoCapping.write(PIN_SERVO_CAPPING,180);
  delay(1500); // Beri waktu servo untuk menyelesaikan putaran 180 derajat dengan torsi penuh

  Serial.println("[4] Stepper: Menaikkan kembali mekanisme capping...");
  naikkanCapping(LANGKAH_TURUN_CAPPING);
  delay(500); // Jeda sebelum servo di-reset agar tidak menyenggol tutup botol yang sudah rapat

  Serial.println("[5] Servo: Reset rotasi kembali ke 0 derajat...");
  servoCapping.write(PIN_SERVO_CAPPING,0);
  delay(1000); // Waktu servo berputar mundur ke posisi awal

  Serial.println("-> Siklus capping selesai! Silakan singkirkan botol.");
  
  // Menunggu botol diambil dari sensor sebelum mengulang siklus
  digitalWrite(PIN_IR_EN, HIGH);
  delay(200);
  while (digitalRead(PIN_IR_OUT) == LOW) {
    delay(100); // Menahan program jika botol lama masih ada
  }
  digitalWrite(PIN_IR_EN, LOW);
  delay(1000); // Jeda sebelum siklus berikutnya
}