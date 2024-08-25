#include <TM1637.h>
#include <Keypad.h>
#include <Wire.h>
#include <RTClib.h>


#define CLK 2
#define DIO 3
TM1637 tm(CLK, DIO);


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
bool isPaused = false;
int pomodoroDuration = 1500; 

void setup() {

  Serial.begin(9600);
  Serial.println("Starting Pomodoro Timer...");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  tm.init(); 
  tm.set(BRIGHT_TYPICAL); 
  tm.clearDisplay();

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  }

  Serial.println("Setup complete. Waiting for time input...");


  inputPomodoroDuration();
}

void loop() {

  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(200); // Debounce delay
    if (isRunning && !isPaused) {
      Serial.println("Pausing timer...");
      isPaused = true;
      digitalWrite(LED_PIN, LOW); 
    } else if (isRunning && isPaused) {
      Serial.println("Resuming timer...");
      isPaused = false;
      startMillis = millis() - (elapsedMillis * 1000); 
      digitalWrite(LED_PIN, HIGH);
    }
  }

  if (isRunning && !isPaused) {
    elapsedMillis = (millis() - startMillis) / 1000; 
    int remainingTime = pomodoroDuration - elapsedMillis;

    
    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;

    tm.display(0, minutes / 10);   
    tm.display(1, minutes % 10);  
    tm.display(2, seconds / 10);   
    tm.display(3, seconds % 10);   

    Serial.print("Time left: ");
    Serial.print(minutes);
    Serial.print(":");
    if (seconds < 10) Serial.print("0");
    Serial.println(seconds);

    if (remainingTime <= 0) {
      Serial.println("Pomodoro complete!");
      isRunning = false;
      digitalWrite(LED_PIN, LOW); 
      tone(BUZZER_PIN, 1000, 2000); 
      storePomodoroSession();
      tm.clearDisplay(); 
      inputPomodoroDuration(); 
    }
  }

  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    handleKeyPress(key);
  }
}

void inputPomodoroDuration() {
  Serial.println("Enter time in minutes (1-60) and press # to confirm:");
  int inputMinutes = 0;
  bool inputComplete = false;

  while (!inputComplete) {
    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        inputMinutes = inputMinutes * 10 + (key - '0');
        tm.display(0, inputMinutes / 10);
        tm.display(1, inputMinutes % 10);
        Serial.print("Current input: ");
        Serial.println(inputMinutes);
      } else if (key == '#') {
        if (inputMinutes >= 1 && inputMinutes <= 60) {
          pomodoroDuration = inputMinutes * 60;
          Serial.print("Pomodoro duration set to ");
          Serial.print(inputMinutes);
          Serial.println(" minutes.");
          inputComplete = true;
        } else {
          Serial.println("Invalid time. Please enter a value between 1 and 60.");
          inputMinutes = 0;
        }
      } else if (key == '*') {
        inputMinutes = 0; 
        tm.clearDisplay();
      }
    }
  }

  startMillis = millis();
  isRunning = true;
  isPaused = false;
  digitalWrite(LED_PIN, HIGH); 
}

void storePomodoroSession() {
  DateTime now = rtc.now();
  Serial.print("Pomodoro completed at: ");
  Serial.println(now.timestamp());
}

void handleKeyPress(char key) {
  if (key == 'A') {
    
    Serial.println("Resetting timer...");
    isRunning = false;
    isPaused = false;
    tm.clearDisplay();
    inputPomodoroDuration(); 
  }
}
