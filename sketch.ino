#include <TM1637Display.h>
#include <Keypad.h>
#include <Wire.h>
#include <RTClib.h>


#define CLK 2
#define DIO 3
TM1637Display display(CLK, DIO);


#define BUTTON_PIN 4
#define LED_PIN 5
#define BUZZER_PIN 16


RTC_DS3231 rtc;


const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9};
byte colPins[COLS] = {10, 11, 12, 13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned long startMillis;
unsigned long elapsedMillis;
bool isRunning = false;
const int pomodoroDuration = 1500;  

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  display.setBrightness(0x0f); 
  if (!rtc.begin()) {

    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  }

  display.showNumberDec(0, true);
}

void loop() {
 
  if (digitalRead(BUTTON_PIN) == LOW && !isRunning) {
    startMillis = millis();
    isRunning = true;
    digitalWrite(LED_PIN, HIGH); 
  }

  if (isRunning) {
    elapsedMillis = (millis() - startMillis) / 1000;
    int remainingTime = pomodoroDuration - elapsedMillis;

    
    display.showNumberDecEx(remainingTime / 60 * 100 + remainingTime % 60, 0x40, true);

    if (remainingTime <= 0) {
      isRunning = false;
      digitalWrite(LED_PIN, LOW); 
      tone(BUZZER_PIN, 1000, 2000); 
      storePomodoroSession();
    }
  }

  char key = keypad.getKey();
  if (key) {
    handleKeyPress(key);
  }
}

void storePomodoroSession() {
  DateTime now = rtc.now();

  Serial.print("Pomodoro completed at: ");
  Serial.println(now.timestamp());
}

void handleKeyPress(char key) {

  if (key == 'A') {
    
    display.showNumberDec(8888);
  }
}
