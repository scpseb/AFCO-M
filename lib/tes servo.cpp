#include <Arduino.h>
#include <Servo.h>

Servo myServo1, myServo2, myServo3= Servo();  
const int servo1Pin = 5; 
const int servo2Pin = 7;
const int servo3Pin = 15;

void setup() {
  Serial.begin(115200);

  myServo1.attach(5, 0);
  myServo2.attach(7, 1);
  myServo3.attach(15,2);

  // Membawa servo ke posisi awal
  myServo1.write(servo1Pin,0);
  myServo2.write(servo2Pin,90);
  myServo3.write(servo3Pin,0);
  // Mencetak instruksi ke Serial Monitor
  Serial.println("\n--- Sistem Servo Siap ---");
}

void loop() {
  // Mengecek apakah ada data yang masuk dari Serial Monitor
   if (Serial.available() > 0) {
     // Membaca satu karakter input
     char input = Serial.read();
    
     // Mengabaikan karakter enter (newline/carriage return) yang terkirim dari Serial Monitor
     if (input == '\n' || input == '\r') {
       return; 
     }

     // Mengecek perintah
     if (input == 'A' || input == 'a') {
       Serial.println("Perintah A: Servo1 Bergerak ke 0 derajat");
       myServo1.write(servo1Pin,45);
       myServo1.printDebug();
     } 
     else if (input == 'B' || input == 'b') {
       Serial.println("Perintah B: Servo1 Bergerak ke 90 derajat");
       myServo1.write(servo1Pin, 90);
       myServo1.printDebug();
     } 
     else if (input == 'C' || input == 'c') {
       Serial.println("Perintah C: Servo2 Bergerak ke 0 derajat");
       myServo2.write(servo2Pin,45);
       myServo2.printDebug();
     }
     else if (input == 'D' || input == 'd') {
       Serial.println("Perintah D: Servo2 Bergerak ke 90 derajat");
       myServo2.write(servo2Pin,90);
       myServo2.printDebug();
     } 
     else if (input == 'E' || input == 'e') {
       Serial.println("Perintah E: Servo3 Bergerak ke 0 derajat");
       myServo3.write(servo3Pin,0);
       myServo3.printDebug();
     }
     else if (input == 'F' || input == 'f') {
      Serial.println("Perintah F: Servo3 Bergerak ke 90 derajat");
      myServo3.write(servo3Pin,90);
      myServo3.printDebug();
     }
     else {
     }
   }
}
