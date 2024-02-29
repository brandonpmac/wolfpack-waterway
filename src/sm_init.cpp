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

#include "led.h"
#include "menu.h"
#include "pins.h"
#include "scheduler.h"
#include "sm.h"
#include "sm_init.h"
#include "sm_types.h"
#include "stepper.h"

bool my_limit_max = false;
bool my_limit_min = false;
static init_state_t init_state = POWER_UP;
static int home_speed = 10000;
static uint32_t init_time = millis();
static int max_home_time = 15000;

static int my_step = 0;
static int current_home_step = 0;
static int current_home_rotation = 0;

void sm_init_entry(sm_event_t last_event) {
  Serial.println("Init entry");

  // enabling tasks
  scheduler.enableTask(5, true, true); // control task

  // changing the display frame
  frame_set(DISPLAY_FRAME_INIT);

  // Setting the led color
  led_color_1_set(GREEN);
  led_color_2_set(BLUE);
}

void sm_init_exit(void) {
  Serial.println("Init exit");

  // disabling tasks

  // turning the led off
  led_color_1_set(OFF);
  led_color_2_set(OFF);

  stepper_speed_set(0, 0, true);
}

void sm_init_periodic(void) {
  switch (init_state) {
  case POWER_UP:
    init_state = HOME_MIN;
    Serial.println("Homing min limit");
    stepper_speed_set(home_speed * -1, 100, false);
    init_time = millis();
    break;

  case HOME_MIN:
    if (digitalRead(SW_LIMIT_MIN) == LOW) {
      init_state = HOME_MAX;
      Serial.println("Homing max limit");
      stepper_speed_set(0, 0, true);
      stepper_speed_set(home_speed, 100, false);
      init_time = millis();
    }
    if ((millis() - init_time) > max_home_time) {
      sm_event_send(SM_EVENT_ERROR_INIT);
    }
    break;

  case HOME_MAX:
    if (digitalRead(SW_LIMIT_MAX) == LOW) {
      init_state = INIT_FINISH;
      Serial.println("Home complete");
      stepper_speed_set(0, 0, true);
    }
    if ((millis() - init_time) > max_home_time) {
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
