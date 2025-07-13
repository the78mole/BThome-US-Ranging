#include <Arduino.h>
#include <ArduinoBLE.h>

#include <Adafruit_NeoPixel.h>
#include <WiFi.h>


#include "version.h"
#include "common.h"
#include "control_ble.h"
#include "improv_ble.h"
#include "bthome.h"
#include "sensors.h"


#if RANGER_MEASURE_LED_WS2812_COUNT > 0
Adafruit_NeoPixel strip(
  RANGER_MEASURE_LED_WS2812_COUNT, 
  RANGER_MEASURE_LED_PIN, NEO_GRB + NEO_KHZ800);

void setPixelColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.show();
}
#endif

void initLED() {
  Serial.println("Initializing LED...");
  #if RANGER_MEASURE_LED_WS2812_COUNT > 0
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    setPixelColor(63, 0, 0); // Set the first pixel to green
    delay(200); 
    setPixelColor(0, 0, 0); // Turn off the LED
  #else
    pinMode(RANGER_MEASURE_LED_PIN, OUTPUT);
    digitalWrite(RANGER_MEASURE_LED_PIN, LOW); // Turn off the LED
  #endif
}


void controlLed(bool state) {
  #if RANGER_MEASURE_LED_WS2812_COUNT > 0
    // If WS2812 LED, then use appropriate library to control it
    // For now, just simulate the LED control
    if (state) {
      setPixelColor(0, 0, 63); // Set the first pixel to green
    } else {
      setPixelColor(0, 0, 0); // Turn off the LED
    }
  #else
    digitalWrite(RANGER_MEASURE_LED_PIN, state ? HIGH : LOW);
  #endif
}

void initGPIO() {
  Serial.println("Initializing GPIO...");
  pinMode(RANGER_POWER_PIN, OUTPUT);
  digitalWrite(RANGER_POWER_PIN, HIGH);

  pinMode(RANGER_MEASURE_TRIGGER_PIN, OUTPUT);
  digitalWrite(RANGER_MEASURE_TRIGGER_PIN, LOW);

  pinMode(RANGER_MEASURE_ECHO_PIN, INPUT);
}


void goToSleep() {
  Serial.printf("Going to sleep... (%d s)\n", SLEEP_DURATION_SECONDS);
  esp_sleep_enable_timer_wakeup(SLEEP_DURATION_SECONDS * 1000000);
  esp_deep_sleep_start();
}

void measureAndSendBthomeData() {
  long distanceMM = measureDistanceMM();
  Serial.printf("Distance measured: %ld mm\n", distanceMM);

  float tempsens;
  temp_sensor_read_celsius(&tempsens);
  Serial.printf("Temperature: %.2f °C\n", tempsens);

  bthomeSendData(distanceMM, tempsens);
}

void setup() {
  // Put your code here, it will run every time, the board is powered 
  // or woken up from deep sleep e.g. by timer

  Serial.begin(115200);
  Serial.printf("Starting BTHome US Distance Meter (%d)...\n", counter++);

  initBLEControl();
  initBLEImprov();
  BLE.advertise();
  delay(250);

  Serial.printf(" Version: %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

  initGPIO();
  initLED();
  initTempSensor();

  measureAndSendBthomeData();

  delay(300); // Give some time for the advertisement to be sent

  
  if (!BLE.connected() && !preventSleep) {    
    goToSleep();
  } else {
    Serial.println("BLE is still connected, not going to sleep -> loop()");
  }
}

uint16_t interval_counter = 0;

void loop() {

  // This will not run in our example, maybe later when 
  // prevent_deep_sleep and OTA is integrated. Then we need to
  // - Disable timer wakeup
  // - Disable deep sleep
  // - Start OTA
  // - Wait for OTA to finish


  if (BLE.connected() || preventSleep) {
    if (BLE.connected() && preventSleep) {
      Serial.print("X");
    } else if (preventSleep) {
      Serial.print("w");
    } else if (BLE.connected()) {
      Serial.print("c");
    } else {
      Serial.print(".");
    }
    if(enableWifi) {

    }
    BLE.poll();
    delay(500);
    if (interval_counter++ % (SLEEP_DURATION_SECONDS * 2) == 0) {
      Serial.print("\nRe-advertise GAP...");
      BLE.advertise();
      delay(250);
      Serial.println("  Measure...");
      measureAndSendBthomeData();
      counter++;
    }
  } else {
    Serial.println("No BLE connection and no preventSleep, going to sleep...");
    goToSleep();
  }
}
