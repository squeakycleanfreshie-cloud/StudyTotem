#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 18
#define TOUCH_PIN 4

Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_MPU6050 mpu;

bool isFlipped = false;
bool lightOn = true;
unsigned long studyStart = 0;
unsigned long totalStudyMs = 0;
bool lastTouchState = false;
unsigned long lastTouchTime = 0;
const unsigned long DEBOUNCE_MS = 300;
const unsigned long PHASE_WARMUP = 5UL * 60 * 1000;
const unsigned long PHASE_FOCUS = 25UL * 60 * 1000;
const unsigned long PHASE_DEEP = 50UL * 60 * 1000;

void setup() {
  Serial.begin(115200);
  led.begin();
  led.setBrightness(180);
  pinMode(TOUCH_PIN, INPUT);
  Wire.begin(8, 9);
  if (!mpu.begin()) {
    while (1) {
      led.setPixelColor(0, led.Color(255, 0, 0));
      led.show(); delay(200);
      led.setPixelColor(0, led.Color(0, 0, 0));
      led.show(); delay(200);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
}

uint8_t breathe(unsigned long periodMs) {
  float phase = (float)(millis() % periodMs) / periodMs;
  float val = (sin(phase * 2.0 * PI - PI / 2.0) + 1.0) / 2.0;
  return (uint8_t)(val * 255);
}

void showStudyColour(unsigned long elapsedMs) {
  if (elapsedMs < PHASE_WARMUP) {
    led.setPixelColor(0, led.Color(0, 80, 255));
  } else if (elapsedMs < PHASE_FOCUS) {
    uint8_t b = breathe(3000);
    led.setPixelColor(0, led.Color(0, b, (uint8_t)(b * 0.3)));
  } else if (elapsedMs < PHASE_DEEP) {
    uint8_t b = breathe(4000);
    led.setPixelColor(0, led.Color((uint8_t)(b * 0.6), 0, b));
  } else {
    uint8_t b = breathe(6000);
    led.setPixelColor(0, led.Color(b, (uint8_t)(b * 0.6), 0));
  }
}

void celebrateFlipBack() {
  for (int i = 0; i < 360; i += 10) {
    uint8_t r = (uint8_t)(sin(radians(i)) * 127 + 128);
    uint8_t g = (uint8_t)(sin(radians(i + 120)) * 127 + 128);
    uint8_t b = (uint8_t)(sin(radians(i + 240)) * 127 + 128);
    led.setPixelColor(0, led.Color(r, g, b));
    led.show();
    delay(18);
  }
}

void loop() {
  unsigned long now = millis();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float z = a.acceleration.z;
  bool currentlyFlipped = (z < -7.0);

  if (currentlyFlipped && !isFlipped) {
    isFlipped = true;
    lightOn = false;
    studyStart = now;
  } else if (!currentlyFlipped && isFlipped) {
    isFlipped = false;
    totalStudyMs += (now - studyStart);
    celebrateFlipBack();
    lightOn = true;
  }

  bool touched = digitalRead(TOUCH_PIN) == HIGH;
  if (touched && !lastTouchState && (now - lastTouchTime > DEBOUNCE_MS)) {
    if (!isFlipped) lightOn = !lightOn;
    lastTouchTime = now;
  }
  lastTouchState = touched;

  if (isFlipped) {
    showStudyColour(now - studyStart);
  } else if (lightOn) {
    led.setPixelColor(0, led.Color(255, 180, 80));
  } else {
    led.setPixelColor(0, led.Color(0, 0, 0));
  }

  led.show();
  delay(20);
}
