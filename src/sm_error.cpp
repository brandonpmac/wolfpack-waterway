/**
 * @file sm_error.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "menu.h"
#include "si_led.h"
#include "sm_error.h"
#include "sm_types.h"

void sm_error_entry(sm_event_t last_event) {
  Serial.println("Error entry");

  // setting led color
  si_led_set(LED_ERROR);

  frame_set(DISPLAY_FRAME_ERROR);
}

void sm_error_exit(void) { Serial.println("Error exit"); }

void sm_error_periodic(void) {}