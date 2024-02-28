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

#include "led.h"
#include "menu.h"
#include "pins.h"
#include "sm.h"
#include "sm_init.h"
#include "sm_types.h"

bool my_limit_max = false;
bool my_limit_min = false;

void sm_init_entry(sm_event_t last_event) {
  Serial.println("Init entry");
  frame_set(DISPLAY_FRAME_INIT);
  led_color_1_set(GREEN);
  led_color_2_set(BLUE);
}

void sm_init_exit(void) { Serial.println("Init exit"); }

void sm_init_periodic(void) {
  if (digitalRead(SW_LIMIT_MAX) == LOW) {
    my_limit_max = true;
  }

  if (digitalRead(SW_LIMIT_MIN) == LOW) {
    my_limit_min = true;
  }

  if ((my_limit_max) && (my_limit_min)) {
    sm_event_send(SM_EVENT_INIT_COMPLETE);
  }
}
