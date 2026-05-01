// Çift HC-SR04 giriş/çıkış sayacı (Arduino Uno)
// Sensör A (kapının dış tarafı): TRIG=2, ECHO=3
// Sensör B (kapının iç tarafı) : TRIG=4, ECHO=5
// Baud Rate: 9600

// ---------------- Pinler ----------------
const uint8_t TRIG_A = 2;
const uint8_t ECHO_A = 3;
const uint8_t TRIG_B = 4;
const uint8_t ECHO_B = 5;

// ---------------- Ayarlar ----------------
const uint16_t MAX_DISTANCE_CM = 100;    // maksimum algılama mesafesi
const uint16_t DETECT_THRESHOLD_CM = 50; // <50 cm => "algılandı"

const unsigned long SENSOR_SAMPLE_PERIOD_MS = 60;  // sensör örnekleme periyodu (çapraz etkiyi azaltır)
const unsigned long SEQUENCE_WINDOW_MS = 800;       // A->B veya B->A bekleme penceresi
const unsigned long LOCKOUT_MS = 1000;              // her sayımdan sonra minimum 1 saniye bekleme
const unsigned long READY_PRINT_PERIOD_MS = 500;    // "sistem hazır" yazma periyodu

const unsigned long OVERLAP_IGNORE_MS = 150;        // iki sensör aynı anda aktifse bu süre dolunca ignore
const unsigned long OVERLAP_COOLDOWN_MS = 300;      // ignore sonrası kısa soğuma (yalancı kenarları engeller)

const uint8_t FILTER_CONSECUTIVE_HITS = 2;          // basit filtre: art arda N kez algılandıysa aktif

// ---------------- Durum Makinesi ----------------
enum State : uint8_t {
  IDLE_READY = 0,
  WAIT_B_AFTER_A,
  WAIT_A_AFTER_B,
  LOCKOUT
};

State state = IDLE_READY;
long countPeople = 0;

unsigned long stateStartMs = 0;
unsigned long lockoutStartMs = 0;
unsigned long lastSampleMs = 0;
unsigned long lastReadyPrintMs = 0;

bool nextSensorIsA = true;

bool prevActiveA = false;
bool prevActiveB = false;

unsigned long bothActiveSinceMs = 0;
unsigned long overlapIgnoreUntilMs = 0;

// Basit filtre sayaçları
uint8_t hitA = 0;
uint8_t hitB = 0;

uint16_t lastDistanceA = 999;
uint16_t lastDistanceB = 999;

uint16_t readDistanceCm(uint8_t trigPin, uint8_t echoPin) {
  // HC-SR04 tetikleme
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 100 cm için ~5800us. Biraz pay bırakalım.
  const unsigned long timeoutUs = 7000UL;
  unsigned long duration = pulseIn(echoPin, HIGH, timeoutUs);

  if (duration == 0) return 999; // timeout / ölçüm yok

  // duration(us) / 58 ~= cm
  uint16_t cm = (uint16_t)(duration / 58UL);
  if (cm > MAX_DISTANCE_CM) return 999;
  return cm;
}

bool updateFilteredActive(uint16_t distanceCm, uint8_t &hitCounter) {
  bool rawActive = (distanceCm < DETECT_THRESHOLD_CM);
  if (rawActive) {
    if (hitCounter < 255) hitCounter++;
  } else {
    hitCounter = 0;
  }
  return (hitCounter >= FILTER_CONSECUTIVE_HITS);
}

void setState(State newState, unsigned long nowMs) {
  state = newState;
  stateStartMs = nowMs;
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_A, OUTPUT);
  pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT);
  pinMode(ECHO_B, INPUT);

  digitalWrite(TRIG_A, LOW);
  digitalWrite(TRIG_B, LOW);

  setState(IDLE_READY, millis());
  Serial.println("Sistem başlatıldı. Sistem hazır...");
}

void loop() {
  unsigned long now = millis();

  // Periyodik sensör örnekleme (A/B sırayla)
  if (now - lastSampleMs >= SENSOR_SAMPLE_PERIOD_MS) {
    lastSampleMs = now;

    if (nextSensorIsA) {
      lastDistanceA = readDistanceCm(TRIG_A, ECHO_A);
    } else {
      lastDistanceB = readDistanceCm(TRIG_B, ECHO_B);
    }
    nextSensorIsA = !nextSensorIsA;
  }

  // Filtrelenmiş aktiflik
  bool activeA = updateFilteredActive(lastDistanceA, hitA);
  bool activeB = updateFilteredActive(lastDistanceB, hitB);

  // Kenar tetikleme: sadece "aktif oldu" anını olay kabul ediyoruz
  bool edgeA = (activeA && !prevActiveA);
  bool edgeB = (activeB && !prevActiveB);

  // Aynı anda iki sensör aktifse: anlık değil, belirli bir süre devam ederse ignore
  if (state != LOCKOUT) {
    if (activeA && activeB) {
      if (bothActiveSinceMs == 0) bothActiveSinceMs = now;
      if (now - bothActiveSinceMs >= OVERLAP_IGNORE_MS) {
        overlapIgnoreUntilMs = now + OVERLAP_COOLDOWN_MS;
        setState(IDLE_READY, now);
      }
    } else {
      bothActiveSinceMs = 0;
    }
  }

  bool overlapIgnoring = (overlapIgnoreUntilMs != 0 && now < overlapIgnoreUntilMs);

  // Lockout: her sayımdan sonra 1 sn yeni sayım yok
  if (state == LOCKOUT) {
    if (now - lockoutStartMs >= LOCKOUT_MS) {
      setState(IDLE_READY, now);
    }
  } else {
    if (!overlapIgnoring) {
    switch (state) {
      case IDLE_READY:
        if (edgeA) {
          setState(WAIT_B_AFTER_A, now);
        } else if (edgeB) {
          setState(WAIT_A_AFTER_B, now);
        }
        break;

      case WAIT_B_AFTER_A:
        // Pencere dolduysa iptal
        if (now - stateStartMs > SEQUENCE_WINDOW_MS) {
          setState(IDLE_READY, now);
          break;
        }
        // A tetiklendikten sonra B tetiklenirse => GİRİŞ
        if (edgeB) {
          countPeople++;
          Serial.print("Giriş yapıldı | Toplam: ");
          Serial.println(countPeople);

          lockoutStartMs = now;
          setState(LOCKOUT, now);
        }
        break;

      case WAIT_A_AFTER_B:
        if (now - stateStartMs > SEQUENCE_WINDOW_MS) {
          setState(IDLE_READY, now);
          break;
        }
        // B tetiklendikten sonra A tetiklenirse => ÇIKIŞ
        if (edgeA) {
          if (countPeople > 0) countPeople--;
          Serial.print("Çıkış yapıldı | Toplam: ");
          Serial.println(countPeople);

          lockoutStartMs = now;
          setState(LOCKOUT, now);
        }
        break;

      default:
        setState(IDLE_READY, now);
        break;
    }
    }
  }

  prevActiveA = activeA;
  prevActiveB = activeB;

  // Sürekli sistem hazır mesajı (spam olmaması için periyodik)
  if (state == IDLE_READY && (now - lastReadyPrintMs >= READY_PRINT_PERIOD_MS)) {
    lastReadyPrintMs = now;
    Serial.print("Sistem hazır | Toplam: ");
    Serial.println(countPeople);
  }
}

