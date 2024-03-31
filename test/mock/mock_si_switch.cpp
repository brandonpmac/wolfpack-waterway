/**
 * @file mock_si_switch.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-03
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include "mock_si_switch.h"
#include "log.h"
#include "si_switch.h"

static bool my_switch_pins[SW_CNT] = {};

bool si_switch_get(si_switch_t sw) {
  if (sw < SW_CNT) {
    return my_switch_pins[sw];
  } else {
    LOG_ERR("Switch index does not exist");
    return false;
  }
}

void si_switch_mock_set_value(si_switch_t sw, bool status) {
  my_switch_pins[sw] = status;
}
