#include "control_ble.h"

BLEService infoService("180A");                       // Device Info Service
BLEStringCharacteristic fwChar("2A26", BLERead, 16);  // Firmware Revision String

#define CTRL_SERVICE_UUID           "12345678-1234-5678-1234-56789abcdef0"
#define CTRL_CHAR_KEEP_AWAKE        "abcdef01-1234-5678-1234-567890000000"
#define CTRL_CHAR_WIFI_EN           "abcdef01-1234-5678-1234-567890000001"

BLEService controlService(CTRL_SERVICE_UUID);
BLEBoolCharacteristic keepAwakeChar(CTRL_CHAR_KEEP_AWAKE, BLERead | BLEWrite);
BLEBoolCharacteristic enableWifiChar(CTRL_CHAR_WIFI_EN, BLERead | BLEWrite);

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

String getMacSuffix() {
  //String mac = WiFi.macAddress();
  String mac = BLE.address();
  return mac.substring(9, 11) + mac.substring(12, 14) + mac.substring(15, 17);
}

void initBLEControl() {
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