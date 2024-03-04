/**
 * @file pins.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief pin definitions
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

#define PIN_ENCODER_CLK (3)
#define PIN_ENCODER_DT (2)

#define PIN_FLOW_SENSOR_1 (18)
#define PIN_FLOW_SENSOR_2 (19)

#define PIN_DISPLAY_I2C_SDA (20)
#define PIN_DISPLAY_I2C_SCL (21)

#define PIN_SW_ENCODER (1)
#define PIN_SW_RUN (6)
#define PIN_SW_LIMIT_MAX (16)
#define PIN_SW_LIMIT_MIN (17)

#define PIN_RELAY_PUMP_1 (4)
#define PIN_RELAY_PUMP_2 (5)

#define PIN_LED_RUN

#define PIN_LED_RED (46)
#define PIN_LED_GREEN (50)
#define PIN_LED_BLUE (52)

#define PIN_STEPPER_DRIVER_ENABLE (44)
#define PIN_STEPPER_DRIVER_MS1 (42)
#define PIN_STEPPER_DRIVER_MS2 (40)
#define PIN_STEPPER_DRIVER_PDN (14)
#define PIN_STEPPER_DRIVER_USART (15)
#define PIN_STEPPER_DRIVER_CLK (34)
#define PIN_STEPPER_DRIVER_STEP (32)
#define PIN_STEPPER_DRIVER_DIR (30)

#endif // PINS_H
