/**
 * @file set_target.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief shell function to change the set point of the tunnel
 * @date 2024-02-17
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "control.h"
#include "menu.h"

int set_target_hndlr(int argc, char **argv) {
  Serial.println(control_setpoint_get());
  return EXIT_SUCCESS;
}
