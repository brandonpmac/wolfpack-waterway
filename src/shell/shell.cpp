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
void shell_init(void) {
  shell.addCommand(F("set"), set_target_hndlr);
  shell.addCommand(F("fc"), flow_correction_hndlr);
  shell.addCommand(F("pid"), set_pid_hndlr);
  shell.addCommand(F("test"), run_test_hndlr);
  shell.addCommand(F("step"), stepper_settings_hndlr);
  shell.addCommand(F("start"), start_hndlr);
  shell.addCommand(F("stop"), stop_hndlr);
  shell.addCommand(F("data"), data_hndlr);
}

/// @brief shell task for the scheduler to run
/// @param
void shell_task(void) { shell.executeIfInput(); }