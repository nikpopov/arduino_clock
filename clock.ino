// Real Time Clock project

/* Use Arduino NANO as microcontroller
Use DS1302 RTC module
All data display on LCD 16x2 (with I2C interface)

Pin Connections
* ARDUINO NANO |   DS1302  |  LCD I2C 16x2
*--------------|-----------|-----------------
*   +5V        |    VIN    |     VIN
*    D4        |    SCL    |
*    D5        |    DAT    |
*    D6        |    RST    |
*    GND       |    GND    |     GND
*   +3.3V      |           |
*    A5        |           |     SCL
*    A4        |           |     SDA
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS1302.h>

// Initialize the LCD with the I2C address 0x27 (common for 16x2 LCDs)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize the DS1302
const int CE_PIN = 6;
const int IO_PIN = 5;
const int SCLK_PIN = 4;
DS1302 rtc(CE_PIN, IO_PIN, SCLK_PIN);

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
  lcd.begin(20, 4);           // Initialize the LCD
  lcd.setBacklight(HIGH);      // Turn on backlight
  lcd.setCursor(0, 0);
  
  // Initialize buttons
  pinMode(setButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  
  // Start RTC
  rtc.halt(false);
  rtc.writeProtect(false);
}

void loop() {
  if (digitalRead(setButton) == HIGH) {
    delay(200);  // Debounce delay
    if (!settingMode) {
      settingMode = true;
      rtc.getDateTime(year, month, day, hour, minute, second);
      settingField = 0;
    } else {
      settingField++;
      if (settingField > 5) {
        settingMode = false;
        rtc.setDateTime(year, month, day, hour, minute, second);
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
  DateTime now = rtc.getDateTime();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(now.year);
  lcd.print("/");
  if (now.month < 10) lcd.print("0");
  lcd.print(now.month);
  lcd.print("/");
  if (now.day < 10) lcd.print("0");
  lcd.print(now.day);

  lcd.setCursor(0, 1);
  if (now.hour < 10) lcd.print("0");
  lcd.print(now.hour);
  lcd.print(":");
  if (now.minute < 10) lcd.print("0");
  lcd.print(now.minute);
  lcd.print(":");
  if (now.second < 10) lcd.print("0");
  lcd.print(now.second);
}
