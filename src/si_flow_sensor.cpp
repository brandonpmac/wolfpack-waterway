/**
 * @file si_flow_sensor.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief flow sensor system interface
 * @date 2024-03-01
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "control.h"
#include "log.h"
#include "si_flow_sensor.h"

#define FLOW_SIZE 32

static uint16_t my_flow_rate[FLOW_SENSOR_CNT] = {0};
static uint32_t my_flow_times[FLOW_SENSOR_CNT] = {millis()};
static uint16_t my_saved_flow_rate[FLOW_SENSOR_CNT][FLOW_SIZE] = {{0}};
static uint32_t my_saved_flow_times[FLOW_SENSOR_CNT][FLOW_SIZE] = {{0}};
static uint16_t my_flow_correction =
    read_from_eeprom(EEPROM_LOCATION_FLOW_CORRECTION);

void ISR_flow_sensor_1_signal(void) {
  if (my_flow_rate[FLOW_SENSOR_1] == UINT16_MAX) {
    LOG_ERR("Flow Sensor 1 value overflow");
  }
  my_flow_rate[FLOW_SENSOR_1] += 1;
};

void ISR_flow_sensor_2_signal(void) {
  if (my_flow_rate[FLOW_SENSOR_2] == UINT16_MAX) {
    LOG_ERR("Flow Sensor 2 value overflow");
  }
  my_flow_rate[FLOW_SENSOR_2] += 1;
};

float si_flow_sensor_rate_get(si_flow_sensor_t sensor) {
  uint16_t flow_rate = my_flow_rate[sensor];
  uint32_t flow_time = millis() - my_flow_times[sensor];
  uint32_t rate_sum = 0;
  uint32_t time_sum = 0;

  my_flow_rate[sensor] = 0;
  my_flow_times[sensor] = millis();

  // Line to produce random data for the tunnel. Only for testing
  // my_flow_rate[sensor] = rand()%(16-14 + 1) + 14;

  for (uint16_t i = 1; i < FLOW_SIZE; i++) {
    rate_sum = rate_sum + my_saved_flow_rate[sensor][i];
    time_sum = time_sum + my_saved_flow_times[sensor][i];

    my_saved_flow_rate[sensor][i - 1] = my_saved_flow_rate[sensor][i];
    my_saved_flow_times[sensor][i - 1] = my_saved_flow_times[sensor][i];
  }
  my_saved_flow_rate[sensor][FLOW_SIZE - 1] = flow_rate;
  my_saved_flow_times[sensor][FLOW_SIZE - 1] = flow_time;

  rate_sum = rate_sum + flow_rate;
  time_sum = time_sum + flow_time;

  return (rate_sum * my_flow_correction) / time_sum;
}

void set_correction_factor(uint16_t value) {
  write_to_eeprom(value, EEPROM_LOCATION_FLOW_CORRECTION);
  my_flow_correction = read_from_eeprom(EEPROM_LOCATION_FLOW_CORRECTION);
  LOG_INF("Flow Correction : %.4u", my_flow_correction);
}
