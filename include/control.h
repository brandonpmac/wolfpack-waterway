/**
 * @file control.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief control loop header file
 * @date 2024-02-08
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>
typedef enum {
  CONTROL_STATUS_IDLE,
  CONTROL_STATUS_PRIME_1,
  CONTROL_STATUS_PRIME_2,
  CONTROL_STATUS_STEADY,
  CONTROL_STATUS_ADJUSTING,
  CONTROL_STATUS_SHUTDOWN_1,
  CONTROL_STATUS_SHUTDOWN_2,

  CONTROL_STATUS_CNT,
  CONTROL_STATUS_ERR = CONTROL_STATUS_CNT,
} control_status_t;
typedef enum {
  PUMPS_BOTH_ACTIVE,
  PUMPS_NONE_ACTIVE,
  PUMPS_FIRST_ACTIVE,
  PUMPS_SECOND_ACTIVE,

  PUMPS_ACTIVE_CNT,
  PUMPS_ERROR = PUMPS_ACTIVE_CNT,
} pump_enum_t;

// getters
pump_enum_t pump_status_get(void);
int32_t tunnel_setpoint_get(void);
int32_t tunnel_speed_get(void);
bool sw_limit_max_get(void);
bool sw_limit_min_get(void);
bool relay_pump_1_get(void);
bool relay_pump_2_get(void);
bool sw_run_get(void);
void tunnel_speed_set(bool, int32_t);

// setters
void tunnel_setpoint_set(int32_t new_setpoint);
void encoder_vent_get(void);
void control_set_pid(int pid, uint16_t new_value);

// interupts
void ISR_flow_sensor_1_SIGNAL(void);
void ISR_flow_sensor_2_SIGNAL(void);

// tasks
void idle_task(void);
void switch_task(void);
void control_task(void);
void flow_sensor_task(void);

#ifdef TEST_BUILD

void pump_status_set(pump_enum_t status);

#endif // TEST_BUILD

#endif // CONTROL_H
