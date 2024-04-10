/**
 * @file control.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief control task header file
 * @date 2024-02-08
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>

uint16_t tunnel_setpoint_get(void);
uint32_t tunnel_speed_get(void);

bool sw_limit_max_get(void);
bool sw_limit_min_get(void);
bool relay_pump_1_get(void);
bool relay_pump_2_get(void);
bool sw_run_get(void);
void tunnel_speed_set(bool, int32_t);

// setters
void tunnel_setpoint_set(uint16_t new_setpoint);
void encoder_vent_get(void);
void control_set_pid(int pid, uint16_t new_value);
int16_t pid_values_get(int value);

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
