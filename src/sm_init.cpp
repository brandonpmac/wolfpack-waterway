/**
 * @file sm_init.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief functions for the init state
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>
#include <lsched.hpp>

#include "log.h"
#include "menu.h"
#include "scheduler.h"
#include "si_led.h"
#include "si_stepper.h"
#include "si_switch.h"
#include "sm.h"
#include "sm_init.h"
#include "sm_types.h"

#define STEPPER_HOME_SPEED (5000)
#define STEPPER_HOME_MAX_TIME (10000)

bool my_limit_max = false;
bool my_limit_min = false;
static init_state_t my_init_state = POWER_UP;
static uint32_t init_time = millis();

void sm_init_entry(sm_event_t last_event) {
  Serial.println("Init entry");

  // enabling tasks
  scheduler.enableTask(5, true, true); // control task

  // changing the display frame
  frame_set(DISPLAY_FRAME_INIT);

  // Setting the led color
  si_led_color_1_set(GREEN);
  si_led_color_2_set(BLUE);
}

void sm_init_exit(void) {
  LOG_INF("Init exit");

  // disabling tasks

  // turning the led off
  si_led_color_1_set(OFF);
  si_led_color_2_set(OFF);

  si_stepper_speed_set(0);
}

void sm_init_periodic(void) {
  switch (my_init_state) {
  case POWER_UP:
    my_init_state = HOME_MIN;
    LOG_INF("Homing min limit");
    si_stepper_speed_set(STEPPER_HOME_SPEED);
    init_time = millis();
    break;

  case HOME_MIN:
    if (si_switch_get(SW_LIMIT_MIN)) {
      my_init_state = HOME_MAX;
      Serial.println("Homing max limit");
      si_stepper_speed_set(0);
      si_stepper_speed_set(-STEPPER_HOME_SPEED);
      init_time = millis();
    } else if (si_switch_get(SW_LIMIT_MAX)) {
      sm_event_send(SM_EVENT_ERROR_INIT);
    }
    if ((millis() - init_time) > STEPPER_HOME_MAX_TIME) {
      sm_event_send(SM_EVENT_ERROR_INIT);
    }
    break;

  case HOME_MAX:
    if (si_switch_get(SW_LIMIT_MAX)) {
      my_init_state = INIT_FINISH;
      Serial.println("Home complete");
      si_stepper_speed_set(0);
    }
    if ((millis() - init_time) > STEPPER_HOME_MAX_TIME) {
      sm_event_send(SM_EVENT_ERROR_INIT);
    }
    break;

  case INIT_FINISH:
    sm_event_send(SM_EVENT_INIT_COMPLETE);
    break;

  default:
    sm_event_send(SM_EVENT_ERROR_INIT);
    break;
  }
}
