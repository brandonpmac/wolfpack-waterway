/**
 * @file si_relay.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-01
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SI_RELAY_H
#define SI_RELAY_H

#include <stdint.h>

typedef enum {
  RELAY_PUMP_1,
  RELAY_PUMP_2,

  RELAY_CNT,
} si_relay_t;

bool si_relay_status_get(si_relay_t relay);

void si_relay_set(si_relay_t relay, bool state);

#endif // SI_RELAY_H