/**
 * @file sm_run.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "control.h"
#include "led.h"
#include "scheduler.h"
#include "sm.h"
#include "sm_run.h"
#include "sm_types.h"

void sm_run_entry(sm_event_t last_event) {
  Serial.println("Run entry");

  // enabling tasks
  scheduler.enableTask(2, true, true); // encoder task
  scheduler.enableTask(3, true, true); // control task

  // Setting led colors
  led_color_1_set(GREEN);
  led_color_2_set(OFF);
}

void sm_run_exit(void) {
  Serial.println("Run exit");

  // disabling tasks
  scheduler.enableTask(2, false, false); // encoder task
  scheduler.enableTask(3, false, false); // control task
}

void sm_run_periodic(void) {
  if (!control_active_get()) {
    sm_event_send(SM_EVENT_IDLE);
  }
}