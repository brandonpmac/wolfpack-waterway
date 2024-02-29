/**
 * @file stepper.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief stepper motor header file
 * @date 2024-02-28
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include <TMC2209.h>

typedef enum {
  STEPPER_COMM_AND_SETUP,
  STEPPER_COMM_NOT_SETUP,
  STEPPER_NOT_COMMUNICATING,
  STEPPER_OFF,
} stepper_status_t;

void stepper_task(void);

void stepper_speed_set(int, int, bool);

int stepper_target_get(void);

int stepper_current_get(void);

#endif // STEPPER_H