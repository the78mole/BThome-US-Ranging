#include "sensors.h"

void initTempSensor() {
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

long measureDistanceMM(int timeout_ms) {
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