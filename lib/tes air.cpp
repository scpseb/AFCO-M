#include <Arduino.h>

// Definisi Pin
const int PIN_SENSOR_AIR = 4; // Pastikan kabel OUT sensor masuk ke sini

void setup() {
  Serial.begin(115200);
  
  // Mengatur pin sebagai input
  pinMode(PIN_SENSOR_AIR, INPUT);
  
  Serial.println("--- Pengujian Sensor XKC-Y25-V ---");
  Serial.println("Tempelkan sensor ke dinding luar wadah berisi air.");
}

void loop() {
  // Membaca status sensor
  int statusSensor = digitalRead(PIN_SENSOR_AIR);
  
  // Menampilkan status ke Serial Monitor
  Serial.print("Status Sensor: ");
  if (statusSensor == HIGH) {
    Serial.println("HIGH (Air Terdeteksi)");
  } else {
    Serial.println("LOW (Tidak Ada Air)");
  }
  
  delay(500); // Jeda agar Serial Monitor tidak terlalu cepat penuh
}