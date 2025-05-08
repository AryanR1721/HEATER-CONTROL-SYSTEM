#include <DHT.h>
#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define DHTPIN 3
#define RED_LED 7
#define GREEN_LED 6
#define SWITCH_PIN 13
#define BUZZER_PIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile float temp = 0.0;
volatile bool newData = false;

enum SystemState {
  IDLE,
  HEATING,
  STABILIZING,
  TARGET_REACHED,
  OVERHEAT
};

SystemState currentState = IDLE;

const float IDLE_THRESHOLD = 25.0;
const float HEATING_THRESHOLD = 30.0;
const float TARGET_TEMP = 35.0;
const float TARGET_TOLERANCE = 1.0;
const float OVERHEAT_THRESHOLD = 38.0;

unsigned long lastBuzzerTime = 0;
bool buzzerOn = false;
const unsigned long buzzerInterval = 2000;

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Timer1.initialize(2000000);  // 2 seconds
  Timer1.attachInterrupt(readTemperature);
}

void loop() {
  int switchState = digitalRead(SWITCH_PIN);

  if (switchState == LOW) {
    // System OFF
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System is OFF");
    delay(500);
    return;
  }

  if (newData) {
    newData = false;

    // State determination
    if (temp < IDLE_THRESHOLD) {
      currentState = IDLE;
    } else if (temp < HEATING_THRESHOLD) {
      currentState = HEATING;
    } else if (temp < TARGET_TEMP - TARGET_TOLERANCE) {
      currentState = STABILIZING;
    } else if (temp <= TARGET_TEMP + TARGET_TOLERANCE) {
      currentState = TARGET_REACHED;
    } else if (temp > OVERHEAT_THRESHOLD) {
      currentState = OVERHEAT;
    }

    switch (currentState) {
      case IDLE:
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        break;
      case HEATING:
      case STABILIZING:
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        break;
      case TARGET_REACHED:
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        break;
      case OVERHEAT:
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, HIGH);
        break;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp, 1);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("State: ");
    lcd.print(getStateText(currentState));
  }

  if (currentState == OVERHEAT) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBuzzerTime >= buzzerInterval) {
      lastBuzzerTime = currentMillis;
      buzzerOn = !buzzerOn;
      digitalWrite(BUZZER_PIN, buzzerOn ? HIGH : LOW);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerOn = false;
  }
}

void readTemperature() {
  float t = dht.readTemperature();
  if (!isnan(t)) {
    temp = t;
    newData = true;
  }
}

const char* getStateText(SystemState state) {
  switch (state) {
    case IDLE: return "Idle";
    case HEATING: return "Heating";
    case STABILIZING: return "Stabilizing";
    case TARGET_REACHED: return "Target OK";
    case OVERHEAT: return "Overheat!";
    default: return "Unknown";
  }
}
