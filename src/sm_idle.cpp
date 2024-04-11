/**
 * @file sm_idle.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief idle state functions
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "control.h"
#include "menu.h"
#include "scheduler.h"
#include "si_led.h"
#include "si_switch.h"
#include "sm.h"
#include "sm_idle.h"
#include "sm_types.h"

void sm_idle_entry(sm_event_t last_event) {
  Serial.println("Idle entry");
  frame_set(DISPLAY_FRAME_IDLE);

  // enabling required tasks
  scheduler.enableTask(2, true, true); // encoder task
  scheduler.enableTask(3, true, true); // switch task
  // scheduler.enableTask(4, true, true); // idle task

  // setting let color
  si_led_set(LED_IDLE);
}

void sm_idle_exit(void) {
  Serial.println("Idle exit");

  // turning the led off
  si_led_set(LED_IDLE);

  scheduler.enableTask(2, false, false); // encoder task
  scheduler.enableTask(3, false, false); // switch task
  // scheduler.enableTask(4, false, false); // idle task
}

void sm_idle_periodic(void) {
  if (sw_run_get()) {
    sm_event_send(SM_EVENT_PRIME);
  }
}