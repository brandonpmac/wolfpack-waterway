/**
 * @file start.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-04-11
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "control.h"

int start_hndlr(int argc, char **argv) {
  set_override(true);
  return EXIT_SUCCESS;
}