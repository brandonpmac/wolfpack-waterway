/**
 * @file set_current.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief flow correction value set shell file
 * @date 2024-04-02
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <SimpleSerialShell.h>

#include "menu.h"
#include "si_flow_sensor.h"

int flow_correction_hndlr(int argc, char **argv) {
  set_correction_factor(atoi(argv[1]));
  return EXIT_SUCCESS;
}