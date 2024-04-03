/**
 * @file set_pid.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-04-02
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "control.h"

int set_pid_hndlr(int argc, char **argv) {
  control_set_pid(atoi(argv[1]), atoi(argv[2]));
  return EXIT_SUCCESS;
}