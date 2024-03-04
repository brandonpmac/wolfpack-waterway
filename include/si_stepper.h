/**
 * @file si_stepper.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief system interface: stepper motor
 * @date 2024-02-28
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SI_STEPPER_H
#define SI_STEPPER_H

#include <stdint.h>

typedef enum {
  STEPPER_COMM_AND_SETUP,
  STEPPER_COMM_NOT_SETUP,
  STEPPER_NOT_COMMUNICATING,
  STEPPER_OFF,
} si_stepper_status_t;

void si_stepper_speed_set(int32_t speed);

int32_t si_stepper_speed_get(void);

#endif // SI_STEPPER_H