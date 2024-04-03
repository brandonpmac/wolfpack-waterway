/**
 * @file sm_shutdown.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-04-02
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "log.h"
#include "menu.h"
#include "scheduler.h"
#include "si_led.h"
#include "si_relay.h"
#include "si_stepper.h"
#include "si_switch.h"
#include "sm.h"
#include "sm_shutdown.h"

#define SHUTDOWN_FIRST_TIME (10000)
#define SHUTDOWN_SECOND_TIME (10000)
#define OPEN_SPEED (5000)

static uint32_t my_time = millis();
static sm_shutdown_state_t my_shutdown_state = SHUTDOWN_ENTRY;
static bool my_relay_pump_1 = si_relay_status_get(RELAY_PUMP_1);
static bool my_relay_pump_2 = si_relay_status_get(RELAY_PUMP_2);

void sm_shutdown_entry(sm_event_t last_event) {
  LOG_INF("Shutdown entry");

  // setting frame
  frame_set(DISPLAY_FRAME_SHUTDOWN_FIRST);

  // setting let color
  si_led_set(LED_SHUTDOWN);

  // enabling required tasks
  scheduler.enableTask(2, true, true); // encoder task
  scheduler.enableTask(3, true, true); // switch task

  // resetting the time
  my_time = millis();
  my_shutdown_state = SHUTDOWN_ENTRY;
}

void sm_shutdown_periodic(void) {
  static bool my_sw_run = si_switch_get(SW_RUN);
  my_relay_pump_1 = si_relay_status_get(RELAY_PUMP_1);
  my_relay_pump_2 = si_relay_status_get(RELAY_PUMP_2);

  // Check if switch was turned off
  if (si_switch_get(SW_RUN)) {
    sm_event_send(SM_EVENT_SHUT_TO_PRIME);
    return;
  }

  if (!si_switch_get(SW_LIMIT_MAX)) {
    si_stepper_speed_set(OPEN_SPEED);
    return;
  } else {
    si_stepper_speed_set(0);
  }

  switch (my_shutdown_state) {
  case SHUTDOWN_ENTRY:
    if (my_relay_pump_1 & my_relay_pump_2) {
      my_shutdown_state = SHUTDOWN_FIRST;
      si_relay_set(RELAY_PUMP_1, LOW);
      my_time = millis();
    } else {
      my_shutdown_state = SHUTDOWN_SECOND;
      si_relay_set(RELAY_PUMP_1, LOW);
      si_relay_set(RELAY_PUMP_2, LOW);
      my_time = millis();
      frame_set(DISPLAY_FRAME_SHUTDOWN_SECOND);
    }
    break;

  case SHUTDOWN_FIRST:
    if ((millis() - my_time) > SHUTDOWN_FIRST_TIME) {
      my_shutdown_state = SHUTDOWN_SECOND;
      si_relay_set(RELAY_PUMP_1, LOW);
      si_relay_set(RELAY_PUMP_2, LOW);
      my_time = millis();
      frame_set(DISPLAY_FRAME_SHUTDOWN_SECOND);
    }
    break;

  case SHUTDOWN_SECOND:
    if ((millis() - my_time) > SHUTDOWN_SECOND_TIME) {
      my_shutdown_state = SHUTDOWN_COMPLETE;
    }
    break;

  case SHUTDOWN_COMPLETE:
    sm_event_send(SM_EVENT_SHUTDOWN_COMPLETE);
    break;

  default:
    LOG_ERR("Unhandled Prime Event");
    sm_event_send(SM_EVENT_ERROR_SHUTDOWN);
    break;
  }
}

void sm_shutdown_exit(void) {
  LOG_INF("Prime exit");

  // turning the led off
  si_led_set(LED_OFF);

  scheduler.enableTask(2, false, false); // encoder task
  scheduler.enableTask(3, false, false); // switch task

  si_stepper_speed_set(0);
}