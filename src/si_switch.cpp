/**
 * @file si_switch.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-01
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "pins.h"
#include "si_switch.h"

static uint8_t my_switch_pins[] = {
    PIN_SW_LIMIT_MIN,
    PIN_SW_LIMIT_MAX,
    PIN_SW_RUN,
    PIN_SW_ENCODER,
};

/// @brief get the state of a swtich
/// @param sw switch index
/// @return bool state of the pin
bool si_switch_get(si_switch_t sw) {
  if (sw < SW_CNT) {
    return !digitalRead(my_switch_pins[sw]);
  } else {
    return false;
  }
}