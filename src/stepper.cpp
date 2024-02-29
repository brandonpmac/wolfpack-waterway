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
#include <Arduino.h>
#include <TMC2209.h>

#include "init.h"
#include "pins.h"
#include "stepper.h"
#include "stepper_driver.h"

static stepper_status_t my_stepper_status = STEPPER_NOT_COMMUNICATING;

static bool stepper_status_check();
static int32_t stepper_current_speed = 0;
static int32_t stepper_target_speed = 0;
static int stepper_acceleration = 0;
static int stepper_max_speed = 50000;
static int stepper_min_speed = stepper_max_speed * -1;

void stepper_speed_set(int new_target_speed, int new_acceleration,
                       bool update_now) {
  if (update_now) {
    stepper_current_speed = new_target_speed;
    stepper_driver.moveAtVelocity(stepper_current_speed * 2);
  }
  stepper_target_speed = new_target_speed;
  stepper_acceleration = new_acceleration;
}

int stepper_target_get(void) { return stepper_target_speed; }

int stepper_current_get(void) { return stepper_current_speed; }

void stepper_task(void) {
  int speed_difference = stepper_target_speed - stepper_current_speed;
  if (speed_difference > 0) {
    if (speed_difference > stepper_acceleration) {
      speed_difference = stepper_acceleration;
    }
    stepper_current_speed = stepper_current_speed + speed_difference;
    stepper_driver.moveAtVelocity(stepper_current_speed * 2);
  } else if (speed_difference < 0) {
    if (abs(speed_difference) > stepper_acceleration) {
      speed_difference = stepper_acceleration * -1;
    }
    stepper_current_speed = stepper_current_speed + speed_difference;
    stepper_driver.moveAtVelocity(stepper_current_speed * 2);
  }
}

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
