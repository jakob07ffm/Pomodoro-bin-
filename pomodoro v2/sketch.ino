#include <TM1637Display.h>
#include <Wire.h>
#include <RTClib.h>
#include <Encoder.h>  // Ensure you have installed the Encoder library

// Pin definitions
#define CLK 2
#define DIO 1
#define ENCODER_CLK 3
#define ENCODER_DT 4
#define BUTTON_PIN 5
#define LED_PIN 6
#define BUZZER_PIN 9
#define RTC_SDA 8
#define RTC_SCL 7

// Timer settings
const int POMODORO_TIME = 25 * 60; // 25 minutes in seconds
const int BREAK_TIME = 5 * 60;     // 5 minutes in seconds

// Global variables
int current_time = POMODORO_TIME;
bool is_pomodoro = true;
bool timer_active = false;
bool timer_done = false;

TM1637Display display(CLK, DIO);
RTC_DS3231 rtc;
Encoder encoder(ENCODER_CLK, ENCODER_DT);

void setup() {
  Serial.begin(9600);
  display.setBrightness(0x0f);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  displayTime();
}

void loop() {
  static unsigned long lastMillis = 0;
  unsigned long currentMillis = millis();

  // Handle button press
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Reset Timer
    current_time = POMODORO_TIME;
    is_pomodoro = true;
    timer_active = true;
    timer_done = false;
    lastMillis = currentMillis;
  }

  // Update timer
  if (timer_active && !timer_done) {
    if (currentMillis - lastMillis >= 1000) {
      lastMillis = currentMillis;
      current_time--;

      if (current_time <= 0) {
        timer_done = true;
        timer_active = false;
        digitalWrite(LED_PIN, HIGH);
        tone(BUZZER_PIN, 1000, 1000); // 1 kHz tone for 1 second
        delay(1000);
        digitalWrite(LED_PIN, LOW);

        if (is_pomodoro) {
          current_time = BREAK_TIME;
          is_pomodoro = false;
        } else {
          current_time = POMODORO_TIME;
          is_pomodoro = true;
        }
      }

      displayTime();
    }
  }

  // Handle rotary encoder
  long encoderPosition = encoder.read();
  if (encoderPosition > 0) {
    current_time += 60; // Increase time by 60 seconds
    encoder.write(0);  // Reset encoder position
    displayTime();
  } else if (encoderPosition < 0) {
    current_time -= 60; // Decrease time by 60 seconds
    encoder.write(0);  // Reset encoder position
    displayTime();
  }
}

void displayTime() {
  int minutes = current_time / 60;
  int seconds = current_time % 60;
  display.showNumberDecEx(minutes * 100 + seconds, 0b01000000, true);
}
