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

#define ENCODER_CLK (3)
#define ENCODER_DT (2)

#define FLOW_SENSOR_1 (18)
#define FLOW_SENSOR_2 (19)

#define DISPLAY_I2C_SDA (20)
#define DISPLAY_I2C_SCL (21)

#define SW_ENCODER (1)
#define SW_RUN (6)
#define SW_LIMIT_MAX (16)
#define SW_LIMIT_MIN (17)

#define PUMP_RELAY_1 (53)
#define PUMP_RELAY_2 (51)

#define LED_RUN

#define LED_RED (46)
#define LED_GREEN (50)
#define LED_BLUE (52)

#endif // PINS_H
