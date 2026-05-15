#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // LCD Kütüphanesi [cite: 1764, 1830]

// LCD Adresi genellikle 0x27'dir, 16 sütun 2 satır [cite: 1830]
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Pin Tanımlamaları [cite: 1409, 1410, 1862, 1863, 1864]
const int trigA = 2; const int echoA = 3; // Sensör A (Dış)
const int trigB = 4; const int echoB = 5; // Sensör B (İç)

// Ayarlanabilir Değerler (Threshold) [cite: 1818, 1819, 1820]
const int ESIK_MESAFE = 80;    // Kapı genişliğine göre optimize edildi [cite: 1806, 1825]
const int BEKLEME_MS = 1000;   // Sayımdan sonraki kilit süresi [cite: 1414, 1731, 1778]
const int PENCERE_MS = 1200;   // İki sensör arası geçiş süresi 

int mevcut = 0;
unsigned long zamanAsimi;

void setup() {
  Serial.begin(9600); // Seri port başlatma [cite: 1334, 1353, 1705]
  lcd.init();         // LCD başlatma [cite: 1833]
  lcd.backlight();    // Arka ışığı aç [cite: 1834]
  
  pinMode(trigA, OUTPUT); pinMode(echoA, INPUT);
  pinMode(trigB, OUTPUT); pinMode(echoB, INPUT);
  
  lcd.setCursor(0, 0);
  lcd.print("Sistem Hazir");
  delay(2000);
  ekranGuncelle();
}

void loop() {
  int mesafeA = mesafeOlc(trigA, echoA);
  delay(50); // Sensörler arası sinyal karışmasını önler [cite: 1775]
  int mesafeB = mesafeOlc(trigB, echoB);

  // GİRİŞ ALGORİTMASI: Önce A, sonra B [cite: 1393, 1412, 1866]
  if (mesafeA < ESIK_MESAFE) {
    zamanAsimi = millis();
    while (millis() - zamanAsimi < PENCERE_MS) {
      if (mesafeOlc(trigB, echoB) < ESIK_MESAFE) {
        mevcut++;
        ekranGuncelle();
        delay(BEKLEME_MS); // Çift saymayı engeller [cite: 1414, 1868]
        break;
      }
    }
  }

  // ÇIKIŞ ALGORİTMASI: Önce B, sonra A [cite: 1394, 1413, 1867]
  if (mesafeB < ESIK_MESAFE) {
    zamanAsimi = millis();
    while (millis() - zamanAsimi < PENCERE_MS) {
      if (mesafeOlc(trigA, echoA) < ESIK_MESAFE) {
        if (mevcut > 0) mevcut--;
        ekranGuncelle();
        delay(BEKLEME_MS);
        break;
      }
    }
  }
}

int mesafeOlc(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long sure = pulseIn(echo, HIGH, 20000); // Zaman aşımı eklendi [cite: 1700]
  int mesafe = sure * 0.034 / 2;
  if (mesafe == 0) return 999; // Hatalı ölçümü yoksay [cite: 1701]
  return mesafe;
}

void ekranGuncelle() {
  lcd.clear(); // [cite: 1844]
  lcd.setCursor(0, 0);
  lcd.print("SINIF MEVCUDU:"); // [cite: 1845]
  lcd.setCursor(0, 1);
  lcd.print(mevcut);
  Serial.print("Guncel Mevcut: "); // [cite: 1846]
  Serial.println(mevcut);
}
