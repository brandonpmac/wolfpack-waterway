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

// a fall on dt while clk is high followed by a fall on clk while dt is low is a
// left turn
// a fall on clk while dt is high followed by a fall on dt while clk
// is low is a right turn

static bool CLK = false;
static bool DT = false;

void ISR_encoder_CLK(void) {
  if (digitalRead(ENCODER_DT) == HIGH) {
    CLK = true;
  }

  if (DT && digitalRead(ENCODER_DT) == LOW) {
    DT = false;
    CLK = false;
    Serial.println("Left turn");
  }
}

void ISR_encoder_DT(void) {
  if (digitalRead(ENCODER_CLK) == HIGH) {
    DT = true;
  }

  if (CLK && digitalRead(ENCODER_CLK) == LOW) {
    DT = false;
    CLK = false;
    Serial.println("Right turn");
  }
}
