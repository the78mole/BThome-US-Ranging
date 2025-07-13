// bthome.h
// This file is part of the BTHome US Distance Meter project.
// It provides the definitions and functions for the BTHome service used in the project.

#ifndef BTHOME_H
#define BTHOME_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <BtHomeV2Device.h>
#include "common.h"
#include "version.h"

extern RTC_DATA_ATTR uint64_t counter; // = 0;

void bthomeSendData(long distanceMM, float tempsens);

#endif // BTHOME_H