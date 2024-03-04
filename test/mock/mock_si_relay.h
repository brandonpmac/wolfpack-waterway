/**
 * @file mock_si_relay.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-03
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef MOCK_SI_RELAY_H
#define MOCK_SI_RELAY_H

#include "si_relay.h"

bool si_relay_status_get(si_relay_t relay);

void si_relay_set(si_relay_t relay, bool state);

#endif // MOCK_SI_RELAY_H