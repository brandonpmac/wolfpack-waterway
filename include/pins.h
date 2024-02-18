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

#define ENCODER_CLK (2)
#define ENCODER_DT (3)

#define FLOW_SENSOR_A (18)
#define FLOW_SENSOR_B (19)

#define DISPLAY_I2C_SDA (20)
#define DISPLAY_I2C_SCL (21)

#define SW_ENCODER (4)
#define SW_RUN (48)
#define SW_LIMIT_MAX (50)
#define SW_LIMIT_MIN (52)

#define RELAY_1 (53)
#define RELAY_2 (51)

#define LED_RUN

#endif // PINS_H
