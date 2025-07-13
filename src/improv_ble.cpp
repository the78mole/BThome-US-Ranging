#include "improv_ble.h"

#define IMPR_SERVICE_UUID           "00467768-6228-2272-4663-277478268000"
#define IMPR_CHAR_CURRENT_STATE     "00467768-6228-2272-4663-277478268001"
#define IMPR_CHAR_ERROR_STATE       "00467768-6228-2272-4663-277478268002"
#define IMPR_CHAR_RPC_COMMAND       "00467768-6228-2272-4663-277478268003"
#define IMPR_CHAR_RPC_RESULT        "00467768-6228-2272-4663-277478268004"
#define IMPR_CHAR_CAPABILITIES      "00467768-6228-2272-4663-277478268005"
#define IMPR_CHAR_DEVICE_INFO       "00467768-6228-2272-4663-277478268006"

BLEService improvService(IMPR_SERVICE_UUID);
BLEByteCharacteristic charState(IMPR_CHAR_CURRENT_STATE, BLERead | BLENotify);
BLEByteCharacteristic charError(IMPR_CHAR_ERROR_STATE, BLERead | BLENotify);
BLECharacteristic charRpcCommand(IMPR_CHAR_RPC_COMMAND, BLEWrite | BLENotify, 128);
BLECharacteristic charRpcResult(IMPR_CHAR_RPC_RESULT, BLENotify, 128);
BLECharacteristic charCapabilities(IMPR_CHAR_CAPABILITIES, BLERead, 1);
BLEStringCharacteristic charDeviceInfo(IMPR_CHAR_DEVICE_INFO, BLERead, 32);

void notifyState(uint8_t state) {
  improv_state = state;
  charState.writeValue(state);
  //charState.notify();
}

void notifyError(uint8_t error) {
  charError.writeValue(error);
  //charError.notify();
}

void notifyResult(const String& result) {
  charRpcResult.writeValue(result.c_str());
  //charRpcResult.notify();
}

void handleRpcCommand(BLEDevice central, BLECharacteristic characteristic) {
  int len = characteristic.valueLength();
  if (len < 3) return; // 1 byte cmd, 1 byte ssid_len, 1 byte pwd_len minimum

  const uint8_t* data = characteristic.value();
  uint8_t cmd = data[0];

  if (cmd == 0x01) {  // Set WiFi settings
    uint8_t ssid_len = data[1];
    uint8_t pwd_len = data[2];
    if (3 + ssid_len + pwd_len > len) return;

    ssid = String((const char*)(data + 3)).substring(0, ssid_len);
    password = String((const char*)(data + 3 + ssid_len)).substring(0, pwd_len);

    Serial.println("Received SSID: " + ssid);
    Serial.println("Received Password: " + password);

    notifyState(STATE_PROVISIONING);
    // Try to connect to WiFi here, simulate success:
    delay(1000); // simulate connection delay
    notifyState(STATE_PROVISIONED);
    notifyResult("192.168.1.42");
  }
  else {
    notifyError(0x01); // unknown command
  }
}

void initBLEImprov() {
  charState.writeValue(improv_state);
  charError.writeValue((uint8_t)0x00);
  charCapabilities.writeValue((uint8_t)0x01); // 0x01 = can set WiFi
  charDeviceInfo.writeValue("RANGER 0.1.0");

  charRpcCommand.setEventHandler(BLEWritten, handleRpcCommand);

  improvService.addCharacteristic(charState);
  improvService.addCharacteristic(charError);
  improvService.addCharacteristic(charRpcCommand);
  improvService.addCharacteristic(charRpcResult);
  improvService.addCharacteristic(charCapabilities);
  improvService.addCharacteristic(charDeviceInfo);

  BLE.addService(improvService);

//   BLE.advertise();
//   delay(250);
//   BLE.stopAdvertise();
}
