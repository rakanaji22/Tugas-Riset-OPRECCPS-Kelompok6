#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BLYNK_TEMPLATE_ID "TMPL6S9ABHNdT"
#define BLYNK_TEMPLATE_NAME "Penyiram Tanaman Otomatis"
#define BLYNK_AUTH_TOKEN "Wb4zybm-uxJEu_iLUnI088XDTLijbAgu"
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

#define PIR_PIN D1
#define RELAY_PIN D3
#define BUZZER_PIN D2
#define MOISTURE_SENSOR_PIN A0
#define SENSOR_THRESHOLD 100 // Ubah threshold ke nilai yang sesuai

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "QD2QlRZ6JcOc590sIE_8rOF2jt6kx-4t";
char ssid[] = "Jangan Dipake!";
char pass[] = "12345678";

bool alarmActive = false;
bool wateringActive = false;
bool Relay = 0;
BlynkTimer timer;

void checkWatering() {
  if (wateringActive) {
    int moistureValue = analogRead(MOISTURE_SENSOR_PIN);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture: ");
    lcd.print("Reading...");

    delay(1000); // Tunggu stabilitas bacaan

    moistureValue = analogRead(MOISTURE_SENSOR_PIN);
    moistureValue = map(moistureValue, 0, 1023, 0, 100);
    Blynk.virtualWrite(V0, moistureValue);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture: ");
    lcd.print(moistureValue);
    lcd.print("%");

    if (moistureValue > SENSOR_THRESHOLD) {
      lcd.setCursor(0, 1);
      lcd.print("Not Watering    ");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Watering Plant  ");
      digitalWrite(RELAY_PIN, HIGH);
      delay(5000);
      digitalWrite(RELAY_PIN, LOW);
    }

    checkPIR(); // Periksa PIR hanya jika tidak ada penyiraman
  }
}

void checkPIR() {
  if (digitalRead(PIR_PIN) == HIGH && !alarmActive) {
    Serial.println("Motion detected");
    digitalWrite(BUZZER_PIN, HIGH);
    alarmActive = true;
    delay(3000);
  } else if (digitalRead(PIR_PIN) == LOW && alarmActive) {
    Serial.println("SYSTEM READY");
    digitalWrite(BUZZER_PIN, LOW);
    alarmActive = false;
    delay(1000);
  }
}

BLYNK_WRITE(V1) {
  Relay = param.asInt();
  
  if (Relay == 1) {
    digitalWrite(RELAY_PIN, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Motor is ON      ");
    wateringActive = true;
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Motor is OFF     ");
    wateringActive = false;
  }
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil Moisture: ");
  lcd.print("Waiting...");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, HIGH);

  timer.setInterval(3000L, checkWatering);
}

void loop() {
  Blynk.run();
  timer.run();
}
