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

#include "led.h"
#include "pins.h"

static led_color_t my_led_1_color = OFF;
static led_color_t my_led_2_color = OFF;
static led_color_t led_current_state = my_led_1_color;
static bool led_past_state = false;

void led_color_1_set(led_color_t new_led_1_color) {
  my_led_1_color = new_led_1_color;
}

void led_color_2_set(led_color_t new_led_2_color) {
  my_led_2_color = new_led_2_color;
}

void led_task(void) {
  if (led_past_state) {
    led_current_state = my_led_1_color;
    led_past_state = false;
  } else {
    led_current_state = my_led_2_color;
    led_past_state = true;
  }

  switch (led_current_state) {
  case OFF:
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    break;

  case RED:
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    break;

  case GREEN:
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
    break;

  case BLUE:
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    break;

  case YELLOW:
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
    break;

  case MAGENTA:
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    break;

  case CYAN:
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    break;

  case WHITE:
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    break;

  default:
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    break;
  }
}