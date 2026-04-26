#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define LED_PIN 18
#define TOUCH_PIN 4

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_MPU6050 mpu;

bool lightOn = true;

void setup() {
  led.begin();
  pinMode(TOUCH_PIN, INPUT);

  Wire.begin(8, 9);
  mpu.begin();
}

void loop() {

  if (digitalRead(TOUCH_PIN) == HIGH) {
    lightOn = false;
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float z = a.acceleration.z;

  if (lightOn && z > 8) {
    led.setPixelColor(0, led.Color(0, 0, 255));
  } else {
    led.setPixelColor(0, led.Color(0, 0, 0));
  }

  led.show();
}
  
