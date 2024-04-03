/**
 * @file sm_prime_first.cpp
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
#include "sm_prime.h"

#define PRIME_FIRST_TIME (10000)
#define PRIME_SECOND_TIME (10000)
#define OPEN_SPEED (5000)

static uint32_t my_time = millis();
static sm_prime_state_t my_prime_state = PRIME_ENTRY;
static bool my_relay_pump_1 = si_relay_status_get(RELAY_PUMP_1);
static bool my_relay_pump_2 = si_relay_status_get(RELAY_PUMP_2);

void sm_prime_entry(sm_event_t last_event) {
  LOG_INF("Prime entry");

  // setting frame
  frame_set(DISPLAY_FRAME_PRIME_FIRST);

  // setting let color
  si_led_set(LED_PRIME);

  // enabling required tasks
  // scheduler.enableTask(2, true, true); // encoder task
  scheduler.enableTask(3, true, true); // switch task

  // Resetting time
  my_time = millis();
  my_prime_state = PRIME_ENTRY;
}

void sm_prime_periodic(void) {
  // Check if switch was turned off
  if (!si_switch_get(SW_RUN)) {
    sm_event_send(SM_EVENT_PRIME_TO_SHUT);
  }

  if (!si_switch_get(SW_LIMIT_MAX)) {
    si_stepper_speed_set(OPEN_SPEED);
    return;
  } else {
    si_stepper_speed_set(0);
  }

  switch (my_prime_state) {
  case PRIME_ENTRY:
    if (!my_relay_pump_1 & !my_relay_pump_2) {
      my_prime_state = PRIME_FIRST;
      si_relay_set(RELAY_PUMP_1, HIGH);
      my_time = millis();
    } else {
      my_prime_state = PRIME_SECOND;
      si_relay_set(RELAY_PUMP_1, HIGH);
      si_relay_set(RELAY_PUMP_2, HIGH);
      my_time = millis();
      frame_set(DISPLAY_FRAME_PRIME_SECOND);
    }
    break;

  case PRIME_FIRST:
    if ((millis() - my_time) > PRIME_FIRST_TIME) {
      my_prime_state = PRIME_SECOND;
      si_relay_set(RELAY_PUMP_1, HIGH);
      si_relay_set(RELAY_PUMP_2, HIGH);
      my_time = millis();
      frame_set(DISPLAY_FRAME_PRIME_SECOND);
    }
    break;

  case PRIME_SECOND:
    if ((millis() - my_time) > PRIME_SECOND_TIME) {
      my_prime_state = PRIME_COMPLETE;
    }
    break;

  case PRIME_COMPLETE:
    sm_event_send(SM_EVENT_PRIME_COMPLETE);
    break;

  case PRIME_SHUTDOWN:
    sm_event_send(SM_EVENT_PRIME_TO_SHUT);
    break;

  default:
    LOG_ERR("Unhandled Prime Event");
    sm_event_send(SM_EVENT_ERROR_PRIME);
    break;
  }
}

void sm_prime_exit(void) {
  LOG_INF("Prime exit");

  // turning the led off
  si_led_set(LED_OFF);

  scheduler.enableTask(2, false, false); // encoder task
  scheduler.enableTask(3, false, false); // switch task

  si_stepper_speed_set(0);
}