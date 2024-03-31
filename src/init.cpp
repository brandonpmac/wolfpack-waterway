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
#include <TMC2209.h>
#include <lsched.hpp>

#include "control.h"
#include "log.h"
#include "menu.h"
#include "pins.h"
#include "shell/shell.h"
#include "si_encoder.h"
#include "si_flow_sensor.h"
#include "si_lcd.h"
#include "sm.h"
#include "stepper_driver.h"

// set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Initialize the Stepper Driver
TMC2209 stepper_driver;
HardwareSerial &serial_stream = Serial3;

// Initialize standard output
FILE f_stdout;
int sput(char c, __attribute__((unused)) FILE *f) { return !Serial.write(c); }

/// @brief Initialization of the microcontroller
void initialize(void) {
  Serial.begin(115200);
  fdev_setup_stream(&f_stdout, sput, nullptr, _FDEV_SETUP_WRITE);
  stdout = &f_stdout;
  stderr = &f_stdout;

  // shell init
  shell.attach(Serial);
  shell_init();

  // initalize display
  lcd.init();
  lcd.setBacklight(1);

  // configure pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_ENCODER_CLK, INPUT);
  pinMode(PIN_ENCODER_DT, INPUT);
  pinMode(PIN_SW_LIMIT_MAX, INPUT_PULLUP);
  pinMode(PIN_SW_LIMIT_MIN, INPUT_PULLUP);
  pinMode(PIN_SW_ENCODER, INPUT_PULLUP);
  pinMode(PIN_SW_RUN, INPUT_PULLUP);
  pinMode(PIN_RELAY_PUMP_1, OUTPUT);
  pinMode(PIN_RELAY_PUMP_2, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);

  // configure interrupts
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), ISR_encoder_CLK,
                  FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), ISR_encoder_DT,
                  FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_SENSOR_1),
                  ISR_flow_sensor_1_signal, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_SENSOR_1),
                  ISR_flow_sensor_2_signal, FALLING);

  // initialize state machine
  sm_init();

  // stepper init

  stepper_driver.setup(serial_stream);
  if (stepper_driver.isSetupAndCommunicating()) {
    LOG_INF("Stepper: Connected");
  }
  stepper_driver.setAllCurrentValues(100, 100, 100);
  stepper_driver.setHardwareEnablePin(PIN_STEPPER_DRIVER_ENABLE);
  stepper_driver.disableAutomaticCurrentScaling();
  stepper_driver.enableAnalogCurrentScaling();
  stepper_driver.disableAutomaticGradientAdaptation();
  stepper_driver.disableCoolStep();
  stepper_driver.disableStealthChop();
  stepper_driver.enable();
  stepper_driver.setMicrostepsPerStep(8);
}
