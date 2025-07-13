// control_ble.h
// This file is part of the BTHome US Distance Meter project.
// It provides the definitions and functions for the BLE control service used in the project.

#ifndef CONTROL_BLE_H
#define CONTROL_BLE_H
#include <Arduino.h>
#include <ArduinoBLE.h>
#include "common.h"
#include "version.h"

static bool preventSleep = false;
static bool enableWifi = false;

String getMacSuffix();
void initBLEControl();


#endif // CONTROL_BLE_H