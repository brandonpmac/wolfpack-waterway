/**
 * @file si_flow_sensor.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief flow sensor header file
 * @date 2024-03-01
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SI_FLOW_SENSOR_H
#define SI_FLOW_SENSOR_H

#include <stdint.h>

typedef enum {
  FLOW_SENSOR_1,
  FLOW_SENSOR_2,

  FLOW_SENSOR_CNT,
} si_flow_sensor_t;

void ISR_flow_sensor_1_signal(void);
void ISR_flow_sensor_2_signal(void);

float si_flow_sensor_rate_get(si_flow_sensor_t sensor);

void set_correction_factor(uint16_t value);

#endif // SI_FLOW_SENSOR_H