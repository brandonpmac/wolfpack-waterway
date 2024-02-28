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

#include "menu.h"
#include "pins.h"
#include "scheduler.h"
#include "sm.h"
#include "sm_idle.h"
#include "sm_types.h"

void sm_idle_entry(sm_event_t last_event) {
  Serial.println("Idle entry");
  frame_set(DISPLAY_FRAME_IDLE);

  scheduler.enableTask(2, true, true);
}

void sm_idle_exit(void) { Serial.println("Idle exit"); }

void sm_idle_periodic(void) {
  if (digitalRead(SW_RUN) == LOW) {
    sm_event_send(SM_EVENT_RUN);
  }
}