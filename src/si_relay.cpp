/**
 * @file si_relay.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief system interface: relays
 * @date 2024-03-01
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "pins.h"
#include "si_relay.h"

static uint8_t my_relay_pins[] = {PIN_RELAY_PUMP_1, PIN_RELAY_PUMP_2};

/// @brief get the status of the relay
/// @param relay index of relay
/// @return bool status of relay
bool si_relay_status_get(si_relay_t relay) {
  if (relay < RELAY_CNT) {
    return digitalRead(my_relay_pins[relay]);
  } else {
    return false;
  }
}

/// @brief set the value of the relay
/// @param relay index of relay
/// @param state state to set
void si_relay_set(si_relay_t relay, bool state) {
  if (relay < RELAY_CNT) {
    digitalWrite(my_relay_pins[relay], state);
  }
}