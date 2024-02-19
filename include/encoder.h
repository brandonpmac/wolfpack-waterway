/**
 * @file encoder.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief encoder header file
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

typedef enum {
  ENCODER_EVENT_NONE,
  ENCODER_EVENT_LEFT,
  ENCODER_EVENT_RIGHT,
  ENCODER_EVENT_BTN,
} encoder_event_t;

encoder_event_t encoder_event_get(void);

void ISR_encoder_CLK(void);

void ISR_encoder_DT(void);

#endif // ENCODER_H
