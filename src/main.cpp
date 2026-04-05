#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

const byte anodePins[] = {2, 3, 4, 5}; 
const byte cathodePins[] = {7, 6, A3, A2, 13, 12, 11, 10, 9, 8}; 

// Додаємо піни для кнопок
const byte hourBtn = A0;
const byte minBtn = A1;

int lastMinute = -1; 

void setup() {
  for (byte i = 0; i < 4; i++) pinMode(anodePins[i], OUTPUT);
  for (byte i = 0; i < 10; i++) pinMode(cathodePins[i], OUTPUT);
  
  // Налаштування кнопок
  pinMode(hourBtn, INPUT_PULLUP);
  pinMode(minBtn, INPUT_PULLUP);
  
  rtc.begin();
}

void displayDigit(byte lampIndex, byte digit) {
  digitalWrite(cathodePins[digit], HIGH);
  digitalWrite(anodePins[lampIndex], HIGH);
  delay(2); 
  digitalWrite(anodePins[lampIndex], LOW);
  digitalWrite(cathodePins[digit], LOW);
  delayMicroseconds(100);
}

// Функція опитування кнопок
void checkButtons() {
  static unsigned long lastPress = 0;
  if (millis() - lastPress < 250) return; // Антибрязк

  bool changed = false;
  DateTime now = rtc.now();
  int h = now.hour();
  int m = now.minute();

  if (digitalRead(hourBtn) == LOW) {
    h = (h + 1) % 24;
    changed = true;
  }

  if (digitalRead(minBtn) == LOW) {
    m = (m + 1) % 60;
    changed = true;
  }

  if (changed) {
    // Оновлюємо час в RTC, скидаючи секунди в 0
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), h, m, 0));
    lastPress = millis();
  }
}

void runSlotMachine() {
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 5; j++) {
      for (byte lamp = 0; lamp < 4; lamp++) {
        displayDigit(lamp, (i + lamp) % 10);
      }
    }
  }
}

void loop() {
  checkButtons(); // Перевіряємо натискання кнопок у кожному циклі

  DateTime now = rtc.now();
  int currentMinute = now.minute();

  if (currentMinute != lastMinute) {
    if (lastMinute != -1) { 
      runSlotMachine();
    }
    lastMinute = currentMinute;
  }

  byte digits[4];
  digits[0] = now.hour() / 10;
  digits[1] = now.hour() % 10;
  digits[2] = now.minute() / 10;
  digits[3] = now.minute() % 10;

  for (byte i = 0; i < 4; i++) {
    displayDigit(i, digits[i]);
  }
}