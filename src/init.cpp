/**
 * @file init.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief initialization functions
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include <Arduino.h>
#include <Wire.h>

#include "encoder.h"
#include "pins.h"
#include <lsched.hpp>

/// @brief Initialization of the microcontroller
void initialize(void) {
  Serial.begin(115200);

  // configure pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);

  // configure interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), ISR_encoder_CLK, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT), ISR_encoder_DT, FALLING);
}
