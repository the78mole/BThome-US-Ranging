#include "sensors.h"
#include "driver/rmt.h"
#include "esp_log.h"

#define RMT_RX_CHANNEL RMT_CHANNEL_2
#define RMT_CLK_DIV    80  // 1 µs at 80 MHz APB clock

void initTempSensor() {
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

float measureTemperatureCelsius() {
  float temperature;
  temp_sensor_read_celsius(&temperature);
  return temperature;
}

void initMeasureDistance() {
Serial.println("Initializing GPIO for Ranger...");
  pinMode(RANGER_POWER_PIN, OUTPUT);
  digitalWrite(RANGER_POWER_PIN, HIGH);

  pinMode(RANGER_MEASURE_TRIGGER_PIN, OUTPUT);
  digitalWrite(RANGER_MEASURE_TRIGGER_PIN, LOW);

  
#ifdef RANGER_USE_RMT
  rmt_driver_uninstall(RMT_RX_CHANNEL);
  pinMode(RANGER_MEASURE_ECHO_PIN, INPUT);

  rmt_config_t rmt_rx;
  rmt_rx.channel = RMT_RX_CHANNEL;
  rmt_rx.gpio_num = (gpio_num_t)RANGER_MEASURE_ECHO_PIN;
  rmt_rx.clk_div = RMT_CLK_DIV;
  rmt_rx.mem_block_num = 1;
  rmt_rx.rmt_mode = RMT_MODE_RX;
  rmt_rx.rx_config.filter_en = true;
  rmt_rx.rx_config.filter_ticks_thresh = 0;
  rmt_rx.rx_config.idle_threshold = 30000; // 30ms max pulse width

  rmt_config(&rmt_rx);
  rmt_driver_install(RMT_RX_CHANNEL, 1000, 0);
#else
  pinMode(RANGER_MEASURE_ECHO_PIN, INPUT);
#endif
}

long measureDistanceMM(int timeout_ms) {
  
  long distance_mm = 0;

  digitalWrite(RANGER_POWER_PIN, HIGH);
  controlLed(true);

#ifdef RANGER_USE_RMT

  // Start Receive
  rmt_item32_t items[1];
  size_t rx_size = 0;
  RingbufHandle_t rb = NULL;
  rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
  rmt_rx_start(RMT_RX_CHANNEL, true);

#endif

digitalWrite(RANGER_MEASURE_TRIGGER_PIN, LOW);
delayMicroseconds(2);
digitalWrite(RANGER_MEASURE_TRIGGER_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(RANGER_MEASURE_TRIGGER_PIN, LOW);

#ifdef RANGER_USE_RMT

  // Wait for response (max 100ms)
  rmt_item32_t* rx_items = (rmt_item32_t*)xRingbufferReceive(rb, &rx_size, pdMS_TO_TICKS(timeout_ms));
  long distance = -1;

  if (rx_items) {
    Serial.printf("Received %d duration items\n", rx_size / sizeof(rmt_item32_t));
    // Only first item is of interest (High time)
    uint32_t duration = rx_items[0].duration0;
    distance = duration * 10 / 58;  // µs → mm
    vRingbufferReturnItem(rb, (void*)rx_items);
  } else {
    Serial.println("No RMT items received");
  }

  rmt_rx_stop(RMT_RX_CHANNEL);

#else

  long duration = pulseIn(RANGER_MEASURE_ECHO_PIN, HIGH, 200000); // Timeout: 100 ms
  distance_mm = duration * 10 / 58;

#endif

  controlLed(false);

  digitalWrite(RANGER_POWER_PIN, LOW);

  // Deactivate measurement timer

  return distance_mm;
}