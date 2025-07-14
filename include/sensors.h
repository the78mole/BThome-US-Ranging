// sensors.h
// This file is part of the BTHome US Distance Meter project.
// It provides the definitions and functions for the temperature sensor used in the project.

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <driver/temp_sensor.h>
#include <common.h>

void initTempSensor();
float measureTemperatureCelsius();

void initMeasureDistance();
long measureDistanceMM(int timeout_ms = 500);

#endif // SENSORS_H