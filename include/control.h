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

#include <Arduino.h>

typedef enum {
  BOTH_ACTIVE,
  BOTH_OFF,
  FIRST_ACTIVE,
  SECOND_ACTIVE,
} pump_enum_t;

extern bool limit_max;
extern bool limit_min;
extern bool run_control;
extern bool pump_1;
extern bool pump_2;

void control_task(void);

#endif // CONTROL_H
