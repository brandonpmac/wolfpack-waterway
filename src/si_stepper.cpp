/**
 * @file stepper.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief stepper motor control
 * @date 2024-02-28
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <TMC2209.h>
#include <stdint.h>

#include "si_stepper.h"
#include "stepper_driver.h"

static si_stepper_status_t my_stepper_status = STEPPER_NOT_COMMUNICATING;

static int32_t my_stepper_speed = 0;

static bool stepper_status_check();

void si_stepper_speed_set(int32_t speed) {
  my_stepper_speed = speed;
  stepper_driver.moveAtVelocity(my_stepper_speed);
}

int32_t si_stepper_speed_get(void) { return my_stepper_speed; }

static bool stepper_status_check(void) {
  // get the stepper status
  if (stepper_driver.isSetupAndCommunicating()) {
    my_stepper_status = STEPPER_COMM_AND_SETUP;
  } else if (stepper_driver.isCommunicatingButNotSetup()) {
    my_stepper_status = STEPPER_COMM_NOT_SETUP;
  } else {
    my_stepper_status = STEPPER_NOT_COMMUNICATING;
  }

  if (my_stepper_status == STEPPER_COMM_AND_SETUP) {
    return true;
  } else {
    return false;
  }
}
