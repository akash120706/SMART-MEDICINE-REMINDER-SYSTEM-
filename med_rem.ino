#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

// Pins
const int LED_PIN = 8;
const int BUZZER_PIN = 9;

// =============================
// MEDICINE TIME
// =============================

int medicineHour = 12;
int medicineMinute = 0;
bool medicinePM = true;

// =============================

bool alarmRunning = false;
unsigned long alarmStart = 0;
int lastAlarmMinute = -1;

void setup() {

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(9600);

  Wire.begin();

  // RTC
  if (!rtc.begin()) {
    Serial.println("RTC ERROR");
    while (1);
  }

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED ERROR");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(20, 20);
  display.println("MEDICINE");

  display.display();

  delay(2000);

  // IMPORTANT:
  // RTC ko computer ke time se set karne ke liye
  // neeche wali line ko ek baar uncomment karke upload karo.
  //
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

// =============================
// LOOP
// =============================

void loop() {

  DateTime now = rtc.now();

  // -----------------------------
  // Convert 24h to 12h
  // -----------------------------

  int hour12 = now.hour();

  bool PM = false;

  if (hour12 >= 12) {
    PM = true;
  }

  if (hour12 == 0) {
    hour12 = 12;
  }
  else if (hour12 > 12) {
    hour12 -= 12;
  }

  // -----------------------------
  // Check medicine time
  // -----------------------------

  if (hour12 == medicineHour &&
      now.minute() == medicineMinute &&
      PM == medicinePM) {

    if (lastAlarmMinute != now.minute()) {

      alarmRunning = true;
      alarmStart = millis();

      lastAlarmMinute = now.minute();
    }
  }

  // -----------------------------
  // Alarm
  // -----------------------------

  if (alarmRunning) {

    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);

    // Alarm for 10 seconds
    if (millis() - alarmStart >= 10000) {

      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      alarmRunning = false;
    }
  }
  else {

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // -----------------------------
  // OLED
  // -----------------------------

  display.clearDisplay();

  // CURRENT TIME
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("CURRENT TIME");

  display.setTextSize(2);
  display.setCursor(0, 12);

  if (hour12 < 10)
    display.print("0");

  display.print(hour12);
  display.print(":");

  if (now.minute() < 10)
    display.print("0");

  display.print(now.minute());

  display.print(":");

  if (now.second() < 10)
    display.print("0");

  display.print(now.second());

  // AM / PM
  display.setTextSize(1);
  display.setCursor(103, 18);

  if (PM)
    display.print("PM");
  else
    display.print("AM");

  // -----------------------------
  // MEDICINE TIME
  // -----------------------------

  display.setCursor(0, 40);

  display.print("MED: ");

  if (medicineHour < 10)
    display.print("0");

  display.print(medicineHour);
  display.print(":");

  if (medicineMinute < 10)
    display.print("0");

  display.print(medicineMinute);

  if (medicinePM)
    display.print(" PM");
  else
    display.print(" AM");

  // -----------------------------
  // STATUS
  // -----------------------------

  display.setCursor(0, 56);

  if (alarmRunning)
    display.print("TAKE MEDICINE!");
  else
    display.print("Waiting...");

  display.display();

  delay(200);
}