/**
 * @file si_encoder.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief system interface: encoder
 * @date 2024-01-30
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#ifndef SI_ENCODER_H
#define SI_ENCODER_H

typedef enum {
  ENCODER_EVENT_NONE,
  ENCODER_EVENT_LEFT,
  ENCODER_EVENT_RIGHT,

  ENCODER_EVENT_CNT,
  ENCODER_EVENT_INVALID = ENCODER_EVENT_CNT,
} si_encoder_event_t;

si_encoder_event_t si_encoder_event_get(void);

void ISR_encoder_CLK(void);

void ISR_encoder_DT(void);

#endif // SI_ENCODER_H
