#include "bthome.h"

RTC_DATA_ATTR uint64_t counter = 0;

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

void bthomeSendData(long distanceMM, float tempsens) {
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