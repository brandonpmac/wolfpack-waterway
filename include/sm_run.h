/**
 * @file sm_init.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SM_RUN_H
#define SM_RUN_H

#include <Arduino.h>

#include "sm.h"

void sm_run_entry(sm_event_t last_event);

void sm_run_exit(void);

void sm_run_periodic(void);

#endif // SM_RUN_H
