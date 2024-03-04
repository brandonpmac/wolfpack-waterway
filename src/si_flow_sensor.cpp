/**
 * @file si_flow_sensor.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-01
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "si_flow_sensor.h"
#include "log.h"

static uint16_t my_flow_rate[FLOW_SENSOR_CNT] = {0};

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

uint16_t si_flow_sensor_rate_get(si_flow_sensor_t sensor) {
  uint16_t flow_rate = my_flow_rate[sensor];
  my_flow_rate[sensor] = 0;
  return flow_rate;
}
