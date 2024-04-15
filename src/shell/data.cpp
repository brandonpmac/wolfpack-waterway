/**
 * @file data.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief 
 * @date 2024-04-12
 * 
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 * 
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "control.h"
#include "sm_run.h"

int data_hndlr(int argc, char **argv) {
    sm_run_data();
  return EXIT_SUCCESS;
}