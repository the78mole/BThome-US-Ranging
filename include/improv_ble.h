// improv_ble.h
// This file is part of the BTHome US Distance Meter project.
// It provides the definitions and functions for the Improv BLE service used in the project.

#ifndef IMPROV_BLE_H
#define IMPROV_BLE_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "common.h"

// Improv States
enum ImprovState : uint8_t {
  STATE_READY = 0,
  STATE_PROVISIONING = 1,
  STATE_PROVISIONED = 2,
  STATE_FAILED = 3
};

static uint8_t improv_state = STATE_READY;
static String ssid = "";
static String password = "";

void initBLEImprov();

#endif // IMPROV_BLE_H