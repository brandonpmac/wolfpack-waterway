/**
 * @file commands.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief commands header file
 * @date 2024-02-17
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef COMMANDS_H
#define COMMANDS_H

int set_target_hndlr(int argc, char **argv);
int set_speed_hndlr(int argc, char **argv);
int set_pid_hndlr(int argc, char **argv);
int run_test_hndlr(int argc, char **argv);
int stepper_settings_hndlr(int argc, char **argv);

#endif // COMMANDS_H