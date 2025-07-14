// common.h
// This file is part of the BTHome US Distance Meter project.
// It provides common definitions and functions used across the project.

#ifndef COMMON_H
#define COMMON_H

#define SLEEP_DURATION_SECONDS  120
//#define RANGER_USE_RMT  // defined => Timer based pulse measurement, else SW based

#define BATTERY_MEASUREMENT_PIN 1

#define RANGER_POWER_PIN    7

#define RANGER_MEASURE_TRIGGER_PIN    9   // High active
#define RANGER_MEASURE_ECHO_PIN       8   // High active

#define RANGER_MEASURE_LED_PIN    48
#define RANGER_MEASURE_LED_WS2812_COUNT  1    // Set to 0, when using a normal LED

const float V_CUTOFF = 3000;  // 0% batteryPercentage
const float V_FULL = 4200;    // 100% batteryPercentage

// Persistent variables use RTC memory to retain their values across deep sleep cycles
//extern RTC_DATA_ATTR uint64_t counter = 0;

extern void controlLed(bool state);

#endif // COMMON_H