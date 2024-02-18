/**
 * @file shell_init.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief shell script to initialzie the shell script
 * @date 2024-02-17
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "shell/commands.h"

/// @brief initializing the functions for the shell
void shell_init(void) { shell.addCommand(F("set_target"), set_target_hndlr); }

/// @brief shell task for the scheduler to run
/// @param
void shell_task(void) { shell.executeIfInput(); }