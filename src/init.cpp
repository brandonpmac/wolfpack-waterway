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
#include <lsched.hpp>

#include "encoder.h"
#include "lcd.h"
#include "menu.h"
#include "pins.h"

// set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27,20,4);  

/// @brief Initialization of the microcontroller
void initialize(void) {
  Serial.begin(115200);

  // initalize display
  lcd.init();

  // configure pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);

  // configure interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), ISR_encoder_CLK, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT), ISR_encoder_DT, FALLING);

  // initialize menu
  menu_init();
}
