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
#include <LiquidCrystal_I2C.h>
#include <SimpleSerialShell.h>
#include <lsched.hpp>

#include "encoder.h"
#include "lcd.h"
#include "menu.h"
#include "pins.h"
#include "shell/shell.h"
#include "sm.h"

// set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

/// @brief Initialization of the microcontroller
void initialize(void) {
  // shell init
  Serial.begin(115200);
  shell.attach(Serial);
  shell_init();

  // initalize display
  lcd.init();

  // configure pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(SW_LIMIT_MAX, INPUT_PULLUP);
  pinMode(SW_LIMIT_MIN, INPUT_PULLUP);
  pinMode(SW_RUN, INPUT_PULLUP);
  pinMode(PUMP_RELAY_1, OUTPUT);
  pinMode(PUMP_RELAY_2, OUTPUT);

  // configure interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), ISR_encoder_CLK, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT), ISR_encoder_DT, FALLING);

  // initialize state machine
  sm_init();
}
