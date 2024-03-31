/**
 * @file mock_si_switch.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-03
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef MOCK_SI_SWITCH_H
#define MOCK_SI_SWITCH_H

#include "si_switch.h"

bool si_switch_get(si_switch_t sw);

void si_switch_mock_set_value(si_switch_t sw, bool status);

#endif // MOCK_SI_SWITCH_H