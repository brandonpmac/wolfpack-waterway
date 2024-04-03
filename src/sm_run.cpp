/**
 * @file sm_run.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-04-02
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include "log.h"
#include "menu.h"
#include "scheduler.h"
#include "si_stepper.h"
#include "si_switch.h"
#include "sm.h"
#include "sm_types.h"

void sm_run_entry(sm_event_t last_event) {
  frame_set(DISPLAY_FRAME_RUN);
  LOG_INF("Run Entry")

  // enable tasks
  scheduler.enableTask(2, true, true); // encoder task
  scheduler.enableTask(3, true, true); // switch task
  scheduler.enableTask(4, true, true); // switch task
  scheduler.enableTask(5, true, true); // flow sensor task
  scheduler.enableTask(6, true, true); // control task
}

void sm_run_exit(void) {
  LOG_INF("Run Exit")
  scheduler.enableTask(2, false, false); // encoder task
  scheduler.enableTask(3, false, false); // switch task
  scheduler.enableTask(4, false, false); // switch task
  scheduler.enableTask(5, false, false); // flow sensor task
  scheduler.enableTask(6, false, false); // control task

  si_stepper_speed_set(0);
}

void sm_run_periodic(void) {
  if (!si_switch_get(SW_RUN)) {
    sm_event_send(SM_EVENT_SHUTDOWN);
    return;
  }
}