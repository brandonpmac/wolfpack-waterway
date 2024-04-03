/**
 * @file run_test.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief 
 * @date 2024-04-03
 * 
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 * 
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "control.h"
#include "sm_run.h"

int run_test_hndlr(int argc, char **argv) {
  uint16_t speed = atoi(argv[1]);
  uint32_t time = atoi(argv[2]);
  sm_run_test(speed, time*1000);
  return EXIT_SUCCESS;
}