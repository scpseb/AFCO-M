# 🏭 CD AFCO-M (Automatic Filling And Capping Oil Machine)

![Status](https://img.shields.io/badge/Status-Active-success)
![Platform](https://img.shields.io/badge/Platform-ESP32--S3-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino%20(PlatformIO)-orange)
![Capstone](https://img.shields.io/badge/Project-Capstone%20Design-purple)

> **Informasi Akademik:** Proyek ini dikembangkan khusus sebagai purwarupa (prototipe) untuk pemenuhan tugas **Capstone Design**.

Proyek **CD AFCO-M** adalah prototipe mesin industri otomatis berbasis mikrokontroler **ESP32-S3** yang dirancang untuk melakukan pengisian cairan (filling) dan penutupan botol (capping) secara berkelanjutan menggunakan mekanisme *Rotary Table* (meja putar). Proyek ini dirancang dengan arsitektur multi-stasiun yang berjalan secara simultan untuk efisiensi produksi.

---

## 👥 Tim Pengembang (Anggota Kelompok)

Proyek Capstone Design ini didesain, dirakit, dan diprogram oleh:

1. **[Sebastian Cahyaputra]** - [101022300014] - *(Hardware & Firmware Engineer)*
2. **[Alya Aulia Putri]** - [101022330153] - *(CAD & System Design)*
3. **[Zaed Al Musthofa]** - [101022300219] - *(Hardware Engineer, System Integrator & QA)*
4. **[Fathi Rachman Marpaung]** - [101022300252] - *(Hardware Engineer & System Integrator)*

Dosen pendamping:
**Husneni Mukhtar, S.Si, M.T., Ph.D** - *Dosen Pendamping 1*
---

## ✨ Fitur Utama

* **Sistem Kontrol Batch Otomatis:** Mesin beroperasi dalam siklus *batch* (1 batch = 8 langkah/botol) yang dipicu oleh satu tombol (*Push Button*).
* **Rotary Table Presisi:** Menggunakan motor stepper Nema 17 dengan driver DRV8825 (Microstepping 1/8) dan rasio transmisi puli 1:3 untuk menghasilkan putaran 45 derajat yang halus dan bertorsi tinggi di setiap stasiun.
* **Stasiun Pengisian (Filling) Cerdas:** * Mendeteksi keberadaan botol menggunakan sensor inframerah (KY-032).
    * Mengontrol suplai tangki sementara secara otomatis menggunakan sensor level cairan non-kontak (XKC-Y25-V).
    * Aktuator keran berbasis motor servo bertenaga (MG996R).
* **Stasiun Penutupan (Capping) Terintegrasi:**
    * Mendeteksi keberadaan botol untuk menghindari gerakan kosong.
    * Mekanisme Z-axis (turun-naik) menggunakan *lead screw* dan motor stepper.
    * Mekanisme putaran (pengencangan tutup) menggunakan motor servo.
* **Manajemen Daya Aman:** Menggunakan *Logic Level Converter* (LLC) untuk komunikasi beda tegangan antara ESP32-S3 (3.3V) dan sensor/aktuator (5V).

---

## 🛠️ Kebutuhan Perangkat Keras (Hardware)

1.  **Mikrokontroler:** ESP32-S3 Dev Module
2.  **Motor & Aktuator:**
    * 2x Motor Stepper Nema 17 (Rotary Table & Capping Z-Axis)
    * 3x Motor Servo MG996R (Keran Utama, Keran Botol, Pemutar Capping)
3.  **Driver & Modul Tambahan:**
    * 2x Driver Stepper DRV8825
    * 1x Logic Level Converter (LLC) 4 Channel / 8 Channel (3.3V ke 5V)
4.  **Sensor:**
    * 2x Sensor Inframerah KY-032 (Deteksi Botol di Stasiun Filling & Capping)
    * 1x Sensor Level Air Digital Non-Kontak XKC-Y25-V
5.  **Catu Daya (Power Supply):**
    * PSU 5V (Minimal 5A) untuk Motor Servo dan Sensor.
    * PSU 12V/24V untuk Motor Stepper.
6.  **Lain-lain:** Push Button, Kapasitor 100µF (untuk DRV8825), Timing Belt, Pulley 1:3.

---

## 🔌 Pemetaan Pin (Wiring Map) ESP32-S3

| Komponen | Nama Pin di Kode | Pin ESP32-S3 | Keterangan |
| :--- | :--- | :--- | :--- |
| **Sistem Utama** | `PIN_TOMBOL` | **GPIO 21** | Tombol Start Batch (Aktif LOW / `INPUT_PULLUP`) |
| **Rotary Table** | `DIR_PIN_ROTARY` | **GPIO 8** | Arah putaran stepper meja |
| | `STEP_PIN_ROTARY` | **GPIO 9** | Sinyal langkah stepper meja |
| **Stasiun Filling** | `PIN_SENSOR_AIR` | **GPIO 4** | Sensor XKC-Y25-V (Level Air Tangki) |
| | `PIN_IR_OUT_FILL` | **GPIO 6** | Sensor KY-032 (Deteksi Botol Filling) |
| | `PIN_IR_EN_FILL` | **GPIO 10** | Enable Sensor KY-032 (Filling) |
| | `PIN_SERVO_1` | **GPIO 5** | Servo Keran Tangki Utama (Channel 0) |
| | `PIN_SERVO_2` | **GPIO 7** | Servo Keran Botol (Channel 1) |
| **Stasiun Capping** | `PIN_IR_OUT_CAP` | **GPIO 18** | Sensor KY-032 (Deteksi Botol Capping) |
| | `PIN_IR_EN_CAP` | **GPIO 17** | Enable Sensor KY-032 (Capping) |
| | `PIN_DIR_CAP` | **GPIO 13** | Arah putaran stepper Z-axis capping |
| | `PIN_STEP_CAP` | **GPIO 14** | Sinyal langkah stepper Z-axis capping |
| | `PIN_SERVO_CAPPING`| **GPIO 15** | Servo Pemutar Tutup Botol (Channel 2) |

---

## 🚀 Panduan Operasi Mesin

1.  **Nyalakan Semua Power Supply:** Pastikan PSU 5V (Servo/Sensor) dan PSU 12V/24V (Stepper) menyala, dan semua jalur Common Ground terhubung dengan baik.
2.  **Inisialisasi Awal (Standby):** Saat mesin pertama kali menyala, ESP32-S3 akan menempatkan semua motor servo di posisi awal (tertutup/standby) dan memastikan *Rotary Table* terkunci pada posisinya.
3.  **Mulai Siklus Batch:** * Tekan **Tombol Start** (di Pin 21). 
    * Sistem akan memulai prosedur pengecekan level air dan mengisi tangki sementara secara otomatis.
    * Meja putar (*rotary table*) akan bergerak sejauh 45 derajat.
    * Sistem akan menginspeksi stasiun *filling* dan *capping* secara bersamaan (simultan). Jika botol terdeteksi oleh sensor inframerah, aktuator akan bekerja sesuai tugas masing-masing.
4.  **Selesai Batch:** Setelah mengulang putaran sebanyak 8 kali (1 siklus penuh / 8 botol), mesin akan menjeda seluruh operasi dan kembali ke mode siaga menunggu tombol ditekan kembali untuk batch berikutnya.

---

## ⚠️ Peringatan Keamanan Perangkat Keras

* **Lonjakan Tegangan (Back-EMF):** Wajib menggunakan kapasitor *electrolytic* (minimal 100µF, 50V) menyilang pada pin `VMOT` dan `GND` di modul DRV8825 untuk menghindari driver terbakar.
* **Perbedaan Logika Tegangan:** Jangan menghubungkan output 5V dari sensor langsung ke pin input ESP32-S3 (yang mentoleransi maksimal 3.3V). Gunakan selalu *Logic Level Converter*.
* **Common Ground:** Sangat krusial untuk menghubungkan pin Ground (GND) dari ESP32-S3, PSU 5V, dan PSU 12V/24V menjadi satu titik hubung yang sama agar sinyal logika dapat terbaca dengan akurat.

---

## 📄 Lisensi

Proyek ini didistribusikan di bawah lisensi MIT. Silakan lihat file `LICENSE` untuk informasi lebih lanjut.

*Didesain dan dikembangkan oleh Kelompok 1 PPT3 Capstone Design CD AFCO-M.*
