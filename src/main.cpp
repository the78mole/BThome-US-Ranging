#include <Arduino.h>

#include <ArduinoBLE.h>
#include <BtHomeV2Device.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <driver/temp_sensor.h>
#include "version.h"

#define SLEEP_DURATION_SECONDS  120
#define BATTERY_MEASUREMENT_PIN 1

#define RANGER_POWER_PIN    7

#define RANGER_MEASURE_TRIGGER_PIN    9   // High active
#define RANGER_MEASURE_ECHO_PIN       8   // High active

#define RANGER_MEASURE_LED_PIN    48
#define RANGER_MEASURE_LED_WS2812_COUNT  1    // Set to 0, when using a normal LED

const float V_CUTOFF = 3000;  // 0% batteryPercentage
const float V_FULL = 4200;    // 100% batteryPercentage

// Persistent variables use RTC memory to retain their values across deep sleep cycles
RTC_DATA_ATTR uint64_t counter = 0;

String getMacSuffix() {
  String mac = WiFi.macAddress();
  return mac.substring(9, 11) + mac.substring(12, 14) + mac.substring(15, 17);
}

BLEService infoService("180A");                       // Device Info Service
BLEStringCharacteristic fwChar("2A26", BLERead, 16);  // Firmware Revision String

BLEService controlService("12345678-1234-5678-1234-56789abcdef0");
BLEBoolCharacteristic keepAwakeChar("abcdef01-1234-5678-1234-567890000000", BLERead | BLEWrite);
bool preventSleep = false;
BLEBoolCharacteristic enableWifiChar("abcdef01-1234-5678-1234-567890000001", BLERead | BLEWrite);
bool enableWifi = false;

void onWriteKeepAwake(BLEDevice central, BLECharacteristic characteristic) {
  preventSleep = keepAwakeChar.value();
  Serial.print("KeepAwake set to: ");
  Serial.println(preventSleep ? "true" : "false");
}

void onWriteEnableWifi(BLEDevice central, BLECharacteristic characteristic) {
  uint8_t val = enableWifiChar.value();
  enableWifi = val != 0;

  Serial.print("BLE: enableWifi set to: ");
  Serial.println(enableWifi ? "true" : "false");

  if (enableWifi) {
    // Starte Webserver, WiFi.begin(...) etc.
    Serial.println("Starting WiFi/WebUI...");
    // WiFi.begin(...);
    // webServer.begin();
  } else {
    // Stoppe Webserver, WiFi.end() etc.
    Serial.println("Stopping WiFi/WebUI...");
    // webServer.stop();
    // WiFi.end();
  }
}

void initBLE() {
  if (!BLE.begin()) {
    // If BLE doesn't start, then just go to sleep
    Serial.println("Failed to initialize BLE!");
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_SECONDS * 1000000);
    esp_deep_sleep_start();
  }

  String suffix = getMacSuffix();
  String localName = "RANGER_" + suffix;
  Serial.printf("Initializing BLE with local name: %s\n", localName.c_str());

  BLE.setDeviceName("RANGER");
  BLE.setLocalName(localName.c_str());
  
  String fwVersion = String(VERSION_MAJOR) + "." + String(VERSION_MINOR) + "." + String(VERSION_PATCH);
  fwChar.writeValue(fwVersion);
  infoService.addCharacteristic(fwChar);
  BLE.addService(infoService);
  
  keepAwakeChar.setValue(preventSleep);
  enableWifiChar.setValue(enableWifi);
  keepAwakeChar.setEventHandler(BLEWritten, onWriteKeepAwake);
  enableWifiChar.setEventHandler(BLEWritten, onWriteEnableWifi);
  controlService.addCharacteristic(keepAwakeChar);
  controlService.addCharacteristic(enableWifiChar);
  BLE.addService(controlService);

  Serial.println("BLE initialized successfully!");
}

void initTempSensor(){
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

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

void sendGapAdvertisement() {
  BLE.advertise();
  delay(250);
}

void sendBThomeAdvertisement(uint8_t advertisementData[], size_t size) {
  BLEAdvertisingData advData;

  BLE.stopAdvertise(); 

  advData.setRawData(advertisementData, size);
  BLE.setAdvertisingData(advData);
  BLE.advertise();
  Serial.println("Raw advertising started!");
  delay(1000);
  BLE.stopAdvertise();
  Serial.println("Raw advertising ended!");
}

void initGPIO() {
  Serial.println("Initializing GPIO...");
  pinMode(RANGER_POWER_PIN, OUTPUT);
  digitalWrite(RANGER_POWER_PIN, HIGH);

  pinMode(RANGER_MEASURE_TRIGGER_PIN, OUTPUT);
  digitalWrite(RANGER_MEASURE_TRIGGER_PIN, LOW);

  pinMode(RANGER_MEASURE_ECHO_PIN, INPUT);
}

long measureDistanceMM(int timeout_ms = 500) {
  digitalWrite(RANGER_POWER_PIN, HIGH);

  controlLed(true);

  // TODO: Reset timer and arm it for pulse width measurement
  
  digitalWrite(RANGER_MEASURE_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(RANGER_MEASURE_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(RANGER_MEASURE_TRIGGER_PIN, LOW);

  long duration = pulseIn(RANGER_MEASURE_ECHO_PIN, HIGH, 200000); // Timeout: 100 ms
  long distance_mm = duration * 10 / 58;

  controlLed(false);

  digitalWrite(RANGER_POWER_PIN, LOW);

  // Deactivate measurement timer

  return distance_mm;
}

void goToSleep() {
  Serial.printf("Going to sleep... (%d s)\n", SLEEP_DURATION_SECONDS);
  esp_sleep_enable_timer_wakeup(SLEEP_DURATION_SECONDS * 1000000);
  esp_deep_sleep_start();
}

void measureAndSendData() {
  long distanceMM = measureDistanceMM();
  Serial.printf("Distance measured: %ld mm\n", distanceMM);
  
  float tempsens;
  temp_sensor_read_celsius(&tempsens);
  Serial.printf("Temperature: %.2f °C\n", tempsens);

  uint8_t advertisementData[MAX_ADVERTISEMENT_SIZE];
  uint8_t size = 0;
  BtHomeV2Device device("RANGER", "WaterRange", false);
  device.addCount_0_4294967295(counter);
  device.addPacketId(counter & 0xFF);
  device.addFirmwareVersion3(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  // device.addRaw(0xF2, &fwVersion, 3); // F2 is the custom data type for firmware version
  // device.addText("v" PROJECT_VERSION);
  device.addDistanceMillimetres(distanceMM);
  device.addTemperature_neg3276_to_3276_Resolution_0_1(tempsens);
  size = device.getAdvertisementData(advertisementData);
  Serial.printf("Advertisement data size: %d bytes\n", size);
  sendBThomeAdvertisement(advertisementData, size);
}

void setup() {
  // Put your code here, it will run every time, the board is powered 
  // or woken up from deep sleep e.g. by timer

  Serial.begin(115200);
  Serial.printf("Starting BTHome US Distance Meter (%d)...\n", counter++);

  initBLE();

  sendGapAdvertisement(); // Start GAP advertisement with no data

  Serial.printf(" Version: %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

  initGPIO();
  initLED();
  initTempSensor();

  measureAndSendData();

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
    BLE.poll();
    delay(500);
    if (interval_counter++ % (SLEEP_DURATION_SECONDS * 2) == 0) {
      Serial.print("\nRe-advertise GAP...");
      sendGapAdvertisement();
      Serial.println("  Measure...");
      measureAndSendData();
      counter++;
    }
  } else {
    Serial.println("No BLE connection and no preventSleep, going to sleep...");
    goToSleep();
  }
}
