/**
 * @file mock_si_relay.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-03
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include "mock_si_relay.h"
#include "si_relay.h"

static bool my_relay_status[RELAY_CNT];

bool si_relay_status_get(si_relay_t relay) {
  if (relay < RELAY_CNT) {
    return my_relay_status[relay];
  } else {
    return false;
  }
}

void si_relay_set(si_relay_t relay, bool state) {
  if (relay < RELAY_CNT) {
    my_relay_status[relay] = state;
  }
}