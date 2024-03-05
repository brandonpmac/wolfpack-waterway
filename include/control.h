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
int control_setpoint_get(void);
int control_process_variable_get(void);
bool limit_max_get(void);
bool limit_min_get(void);
bool control_active_get(void);

// setters
void control_setpoint_set(int);
void control_process_variable_set(int);

// interupts
void ISR_flow_sensor_1_SIGNAL(void);
void ISR_flow_sensor_2_SIGNAL(void);

// tasks
void control_task(void);
void switch_task(void);

#ifdef TEST_BUILD

void pump_status_set(pump_enum_t status);

#endif // TEST_BUILD

#endif // CONTROL_H
