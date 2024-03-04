/**
 * @file mock_si_encoder.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-03
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include "si_encoder.h"

static si_encoder_event_t my_encoder_event = ENCODER_EVENT_INVALID;

si_encoder_event_t si_encoder_event_get(void) { return my_encoder_event; }

void si_encoder_event_mock_set_value(si_encoder_event_t event) {
  my_encoder_event = event;
}