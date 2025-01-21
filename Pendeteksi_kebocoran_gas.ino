#define BLYNK_TEMPLATE_ID "TMPL6nC6Gmxk0"
#define BLYNK_TEMPLATE_NAME "Pendeteksi Kebocoran gas"
#define BLYNK_AUTH_TOKEN "bC-v-Nl_ptlmYLzvC7aBYZopDgDhd93m"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "A12 milik Febri";
char pass[] = "febri11590";

#define GAS_SENSOR_PIN 34
#define RELAY_PIN 25
#define BUZZER_PIN 26
#define LED_PIN 27

float gasValue;
BlynkTimer timer;

// Fungsi untuk menghitung rata-rata dari beberapa pembacaan sensor
float averageSensorReading(int pin, int numReadings) {
  float sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(pin);
    delay(10); // Jeda singkat antar pembacaan
  }
  return sum / numReadings;
}

void sendGasData() {
  // Baca nilai sensor dan hitung konsentrasi gas
  int sensorValue = averageSensorReading(GAS_SENSOR_PIN, 10); // Ambil rata-rata dari 10 pembacaan
  gasValue = (sensorValue / 4095.0) * 100;

  Serial.print("Gas Concentration: ");
  Serial.print(gasValue);
  Serial.println(" %");

  Blynk.virtualWrite(V0, gasValue); // Kirim nilai gas ke Blynk (Gauge)

  String status; // Variabel String untuk menyimpan status
  if (gasValue < 10) {
    digitalWrite(LED_PIN, HIGH); // LED menyala
    digitalWrite(BUZZER_PIN, LOW);  // Buzzer mati
    digitalWrite(RELAY_PIN, HIGH); // Relay ON (Katup gas terbuka)
    status = "Aman";
  } else if (gasValue >= 10 && gasValue < 20) {
    digitalWrite(RELAY_PIN, HIGH); // Relay ON (Katup gas terbuka)
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      tone(BUZZER_PIN, 1000);
      delay(50);
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      delay(50);
    }
    status = "Peringatan";
  } else { // gasValue >= 20
    digitalWrite(RELAY_PIN, LOW); 
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN, HIGH);
      tone(BUZZER_PIN, 2000);
      delay(30);
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      delay(30);
    }
    status = "Bahaya";
  }

  Blynk.virtualWrite(V1, status); 
}

void setup() {
  Serial.begin(115200);
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // Pastikan relay ON saat mulai
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  while (!Blynk.connected()) {
    delay(50);
    Serial.print("Blynk connecting...");
  }
  Serial.println("Blynk connected");

  timer.setInterval(1000L, sendGasData);
}

void loop() {
  Blynk.run();
  timer.run();
}