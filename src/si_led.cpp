/**
 * @file led.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief functions for the led task
 * @date 2024-02-28
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "pins.h"
#include "si_led.h"
#include "sm_types.h"

static si_led_color_t my_led_1_color = OFF;
static si_led_color_t my_led_2_color = OFF;
static si_led_color_t my_led_current_state = my_led_1_color;
static bool my_led_past_state = false;

void si_led_set(si_led_state_t state) {
  switch (state) {
  case LED_OFF:
    my_led_1_color = OFF;
    my_led_2_color = OFF;
    break;

  case LED_INIT:
    my_led_1_color = BLUE;
    my_led_2_color = GREEN;
    break;

  case LED_IDLE:
    my_led_1_color = BLUE;
    my_led_2_color = OFF;
    break;

  case LED_ERROR:
    my_led_1_color = RED;
    my_led_2_color = OFF;
    break;

  case LED_PRIME:
    my_led_1_color = YELLOW;
    my_led_2_color = OFF;
    break;

  case LED_SHUTDOWN:
    my_led_1_color = MAGENTA;
    my_led_2_color = OFF;
    break;

  case LED_RUN:
    my_led_1_color = GREEN;
    my_led_2_color = OFF;
    break;

  default:
    my_led_1_color = OFF;
    my_led_2_color = OFF;
    break;
  }
}

void si_led_task(void) {
  if (my_led_past_state) {
    my_led_current_state = my_led_1_color;
    my_led_past_state = false;
  } else {
    my_led_current_state = my_led_2_color;
    my_led_past_state = true;
  }

  switch (my_led_current_state) {
  case OFF:
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_BLUE, HIGH);
    break;

  case RED:
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_BLUE, HIGH);
    break;

  case GREEN:
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_BLUE, HIGH);
    break;

  case BLUE:
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_BLUE, LOW);
    break;

  case YELLOW:
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_BLUE, HIGH);
    break;

  case MAGENTA:
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_BLUE, LOW);
    break;

  case CYAN:
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_BLUE, LOW);
    break;

  case WHITE:
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_BLUE, LOW);
    break;

  default:
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_BLUE, HIGH);
    break;
  }
}