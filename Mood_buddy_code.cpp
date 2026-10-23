#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

const int TRIG_PIN = 5;
const int ECHO_PIN = 6;
const int BUZZER_PIN = 8;

const float FAR_DISTANCE = 40.0;
const float HAPPY_DISTANCE = 15.0;
const float SHY_DISTANCE = 7.0;

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);

enum Mood { SLEEPING, WAKING, HAPPY, SHY };

Mood mood = SLEEPING;
Mood previousMood = SLEEPING;

unsigned long wakeStart = 0;
unsigned long lastBeep = 0;
unsigned long lastAnimation = 0;
bool wakeBeepDone = false;
bool blink = false;

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration =
    pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration == 0) return -1;

  return duration * 0.0343 / 2.0;
}

void drawEars() {
  display.drawLine(25, 23, 35, 7, SSD1306_WHITE);
  display.drawLine(35, 7, 47, 23, SSD1306_WHITE);

  display.drawLine(81, 23, 93, 7, SSD1306_WHITE);
  display.drawLine(93, 7, 103, 23, SSD1306_WHITE);

  display.drawLine(33, 19, 35, 12, SSD1306_WHITE);
  display.drawLine(35, 12, 41, 20, SSD1306_WHITE);

  display.drawLine(87, 20, 93, 12, SSD1306_WHITE);
  display.drawLine(93, 12, 95, 19, SSD1306_WHITE);
}

void drawWhiskers() {
  display.drawLine(29, 39, 13, 35, SSD1306_WHITE);
  display.drawLine(28, 43, 10, 43, SSD1306_WHITE);
  display.drawLine(29, 47, 14, 51, SSD1306_WHITE);

  display.drawLine(99, 39, 115, 35, SSD1306_WHITE);
  display.drawLine(100, 43, 118, 43, SSD1306_WHITE);
  display.drawLine(99, 47, 114, 51, SSD1306_WHITE);
}

void drawSleeping() {
  display.clearDisplay();
  drawEars();

  display.drawLine(36, 31, 43, 35, SSD1306_WHITE);
  display.drawLine(43, 35, 50, 31, SSD1306_WHITE);

  display.drawLine(78, 31, 85, 35, SSD1306_WHITE);
  display.drawLine(85, 35, 92, 31, SSD1306_WHITE);

  drawWhiskers();

  display.fillTriangle(
    61, 39, 67, 39, 64, 43, SSD1306_WHITE
  );

  display.drawLine(64, 43, 60, 47, SSD1306_WHITE);
  display.drawLine(60, 47, 56, 44, SSD1306_WHITE);
  display.drawLine(64, 43, 68, 47, SSD1306_WHITE);
  display.drawLine(68, 47, 72, 44, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(99, 4);
  display.print("zZ");

  display.display();
}


void drawHappyEyes() {
  // Large round eyes
  display.fillCircle(43, 32, 10, SSD1306_WHITE);
  display.fillCircle(85, 32, 10, SSD1306_WHITE);

  // Dark pupils
  display.fillCircle(45, 33, 5, SSD1306_BLACK);
  display.fillCircle(87, 33, 5, SSD1306_BLACK);

  // Sparkles
  display.fillCircle(41, 29, 2, SSD1306_WHITE);
  display.fillCircle(83, 29, 2, SSD1306_WHITE);
  display.drawPixel(49, 36, SSD1306_WHITE);
  display.drawPixel(91, 36, SSD1306_WHITE);
}


void drawWaking() {
  display.clearDisplay();
  drawEars();

  drawHappyEyes();
  drawWhiskers();

  display.fillRoundRect(
    59, 41, 11, 12, 4, SSD1306_WHITE
  );

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(102, 4);
  display.print("!");

  display.display();
}

void drawHappy() {
  display.clearDisplay();
  drawEars();

  display.drawLine(31, 34, 39, 27, SSD1306_WHITE);
  display.drawLine(39, 27, 47, 34, SSD1306_WHITE);
  display.drawLine(47, 34, 51, 30, SSD1306_WHITE);

  display.drawLine(77, 34, 85, 27, SSD1306_WHITE);
  display.drawLine(85, 27, 93, 34, SSD1306_WHITE);
  display.drawLine(93, 34, 97, 30, SSD1306_WHITE);

  display.drawLine(64, 39, 64, 44, SSD1306_WHITE);
  display.drawLine(64, 44, 57, 44, SSD1306_WHITE);
  display.drawLine(57, 44, 58, 48, SSD1306_WHITE);
  display.drawLine(58, 48, 64, 51, SSD1306_WHITE);
  display.drawLine(64, 51, 70, 48, SSD1306_WHITE);
  display.drawLine(70, 48, 71, 44, SSD1306_WHITE);
  display.drawLine(71, 44, 64, 44, SSD1306_WHITE);

  display.drawLine(40, 44, 45, 46, SSD1306_WHITE);
  display.drawLine(83, 46, 88, 44, SSD1306_WHITE);

  display.display();
}


void drawShy() {
  display.clearDisplay();
  drawEars();

  drawHappyEyes();

  display.drawPixel(31, 26, SSD1306_WHITE);
  display.drawPixel(53, 24, SSD1306_WHITE);
  display.drawPixel(73, 24, SSD1306_WHITE);
  display.drawPixel(97, 26, SSD1306_WHITE);

  drawWhiskers();

  display.fillRoundRect(
    57, 41, 14, 15, 5, SSD1306_WHITE
  );
  display.fillRoundRect(
    60, 43, 8, 6, 3, SSD1306_BLACK
  );

  display.drawLine(36, 45, 42, 48, SSD1306_WHITE);
  display.drawLine(43, 45, 47, 48, SSD1306_WHITE);
  display.drawLine(81, 48, 85, 45, SSD1306_WHITE);
  display.drawLine(86, 48, 92, 45, SSD1306_WHITE);

  display.display();
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(TRIG_PIN, LOW);
  noTone(BUZZER_PIN);

  Serial.begin(9600);
  Wire.begin();

  if (!display.begin(
        SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED not found!");
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.display();

  drawSleeping();
}

void loop() {
  float distance = getDistance();

  Serial.print("Distance: ");
  if (distance < 0) {
    Serial.println("No echo");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }

  if (distance < 0 || distance > FAR_DISTANCE) {
    mood = SLEEPING;
  } else if (distance <= SHY_DISTANCE) {
    mood = SHY;
  } else if (distance <= HAPPY_DISTANCE) {
    mood = HAPPY;
  } else {
    mood = WAKING;
  }

  if (mood != previousMood) {
    noTone(BUZZER_PIN);
    
    wakeStart = millis();
    lastBeep = millis();
    lastAnimation = 0;
    wakeBeepDone = false;
    blink = false;
  }

  if (mood == SLEEPING) {
    noTone(BUZZER_PIN);
    drawSleeping();
  }

  else if (mood == WAKING) {
    if (!wakeBeepDone) {
      tone(BUZZER_PIN, 1200);
      delay(100);
      noTone(BUZZER_PIN);
      wakeBeepDone = true;
    }

    if (millis() - lastAnimation >= 180) {
      lastAnimation = millis();
      blink = !blink;

      if (blink) {
        drawSleeping();
      } else {
        drawWaking();
      }
    }
  }

  else if (mood == HAPPY) {
    if (millis() - lastBeep >= 1000) {
      tone(BUZZER_PIN, 1500);
      delay(70);
      noTone(BUZZER_PIN);
      lastBeep = millis();
    }

    drawHappy();
  }

  else if (mood == SHY) {
    if (millis() - lastBeep >= 180) {
      tone(BUZZER_PIN, 1900);
      delay(55);
      noTone(BUZZER_PIN);
      lastBeep = millis();
    }

    drawShy();
  }

  previousMood = mood;
  delay(40);
}