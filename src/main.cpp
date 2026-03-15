#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

const byte anodePins[] = {2, 3, 4, 5}; 
const byte cathodePins[] = {7, 6, A3, A2, 13, 12, 11, 10, 9, 8}; 

int lastMinute = -1; // Змінна для відстеження зміни часу

void setup() {
  for (byte i = 0; i < 4; i++) pinMode(anodePins[i], OUTPUT);
  for (byte i = 0; i < 10; i++) pinMode(cathodePins[i], OUTPUT);
  rtc.begin();
  
  // Розкоментуй для налаштування часу (+30 сек компенсації затримки)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(0, 0, 0, 30));
}

void displayDigit(byte lampIndex, byte digit) {
  digitalWrite(cathodePins[digit], HIGH);
  digitalWrite(anodePins[lampIndex], HIGH);
  delay(2); // Трохи зменшив для плавності
  digitalWrite(anodePins[lampIndex], LOW);
  digitalWrite(cathodePins[digit], LOW);
  delayMicroseconds(100);
}

// Ефект "Слот-машина"
void runSlotMachine() {
  // Робимо 15 швидких переборів
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 5; j++) { // Внутрішній цикл для видимості світіння
      for (byte lamp = 0; lamp < 4; lamp++) {
        displayDigit(lamp, (i + lamp) % 10);
      }
    }
  }
}

void loop() {
  DateTime now = rtc.now();
  int currentMinute = now.minute();

  // ПЕРЕВІРКА: якщо хвилина змінилася — запускаємо слот-машину
  if (currentMinute != lastMinute) {
    if (lastMinute != -1) { // Не запускати при самому ввімкненні
      runSlotMachine();
    }
    lastMinute = currentMinute;
  }

  byte digits[4];
  digits[0] = now.hour() / 10;
  digits[1] = now.hour() % 10;
  digits[2] = now.minute() / 10;
  digits[3] = now.minute() % 10;

  // Основний вивід часу
  for (byte i = 0; i < 4; i++) {
    displayDigit(i, digits[i]);
  }
}