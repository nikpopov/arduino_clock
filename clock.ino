// Real Time Clock project

/* Use Arduino UNO as microcontroller
Use DS1302 RTC module
All data display on LCD 16x2 (with I2C interface)

Pin Connections
* ARDUINO UNO  |   DS1302  |  LCD I2C 16x2
*--------------|-----------|-----------------
*   +5V        |           |     VIN
*    D4        |    SCL    |
*    D5        |    DAT    |
*    D6        |    RST    |
*    GND       |    GND    |     GND
*   +3.3V      |    VIN    |
*    A5        |           |     SCL
*    A4        |           |     SDA
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <iarduino_RTC.h>

// Initialize the LCD with the I2C address 0x27 (common for 16x2 LCDs)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize the iarduino_RTC with DS1302
iarduino_RTC rtc(RTC_DS1302);

// Buttons
const int setButton = 7;
const int upButton = 8;
const int downButton = 9;

// Variables for time and date
int year, month, day, hour, minute, second;
bool settingMode = false;
int settingField = 0;

void setup() {
  // Initialize LCD
  lcd.init();
  // lcd.backlight();
  
  // Initialize buttons
  pinMode(setButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  
  // Start RTC
  rtc.begin();
  
  // Read initial time
  rtc.gettime();
}

void loop() {
  if (digitalRead(setButton) == HIGH) {
    delay(200);  // Debounce delay
    if (!settingMode) {
      settingMode = true;
      rtc.gettime();
      year = rtc.year;
      month = rtc.month;
      day = rtc.day;
      hour = rtc.Hours;
      minute = rtc.minutes;
      second = rtc.seconds;
      settingField = 0;
    } else {
      settingField++;
      if (settingField > 5) {
        settingMode = false;
        rtc.settime(year, month, day, hour, minute, second);
      }
    }
  }

  if (settingMode) {
    if (digitalRead(upButton) == HIGH) {
      adjustTime(1);
      delay(200);  // Debounce delay
    } else if (digitalRead(downButton) == HIGH) {
      adjustTime(-1);
      delay(200);  // Debounce delay
    }
    displaySettingMode();
  } else {
    displayTime();
  }
}

void adjustTime(int delta) {
  switch (settingField) {
    case 0: year += delta; break;
    case 1: month = constrain(month + delta, 1, 12); break;
    case 2: day = constrain(day + delta, 1, 31); break;
    case 3: hour = constrain(hour + delta, 0, 23); break;
    case 4: minute = constrain(minute + delta, 0, 59); break;
    case 5: second = constrain(second + delta, 0, 59); break;
  }
}

void displaySettingMode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Time/Date");

  lcd.setCursor(0, 1);
  if (settingField <= 2) {
    lcd.print(year);
    lcd.print("/");
    if (month < 10) lcd.print("0");
    lcd.print(month);
    lcd.print("/");
    if (day < 10) lcd.print("0");
    lcd.print(day);
  } else {
    if (hour < 10) lcd.print("0");
    lcd.print(hour);
    lcd.print(":");
    if (minute < 10) lcd.print("0");
    lcd.print(minute);
    lcd.print(":");
    if (second < 10) lcd.print("0");
    lcd.print(second);
  }
  lcd.setCursor(settingField * 3, 1);  // Highlight the field being set
  lcd.blink();
}

void displayTime() {
  rtc.gettime();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(rtc.year);
  lcd.print("/");
  if (rtc.month < 10) lcd.print("0");
  lcd.print(rtc.month);
  lcd.print("/");
  if (rtc.day < 10) lcd.print("0");
  lcd.print(rtc.day);

  lcd.setCursor(0, 1);
  if (rtc.hours < 10) lcd.print("0");
  lcd.print(rtc.hours);
  lcd.print(":");
  if (rtc.minutes < 10) lcd.print("0");
  lcd.print(rtc.minutes);
  lcd.print(":");
  if (rtc.seconds < 10) lcd.print("0");
  lcd.print(rtc.seconds);
}
