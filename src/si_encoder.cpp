/**
 * @file encoder.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief A set of functions to interpret an encoder.
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include <Arduino.h>

#include "pins.h"
#include "si_encoder.h"

// a fall on dt while clk is high followed by a fall on clk while dt is low is a
// left turn
// a fall on clk while dt is high followed by a fall on dt while clk
// is low is a right turn

static bool my_clk = false;
static bool my_dt = false;

static si_encoder_event_t my_encoder_event = ENCODER_EVENT_NONE;

si_encoder_event_t si_encoder_event_get(void) {
  si_encoder_event_t encoder_event = my_encoder_event;
  my_encoder_event = ENCODER_EVENT_NONE;
  return encoder_event;
}

void ISR_encoder_CLK(void) {
  if (digitalRead(PIN_ENCODER_DT) == HIGH) {
    my_clk = true;
  }

  if (my_dt && digitalRead(PIN_ENCODER_DT) == LOW) {
    my_dt = false;
    my_clk = false;
    my_encoder_event = ENCODER_EVENT_LEFT;
  }
}

void ISR_encoder_DT(void) {
  if (digitalRead(PIN_ENCODER_CLK) == HIGH) {
    my_dt = true;
  }

  if (my_clk && digitalRead(PIN_ENCODER_CLK) == LOW) {
    my_dt = false;
    my_clk = false;
    my_encoder_event = ENCODER_EVENT_RIGHT;
  }
}
