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

#ifndef SM_INIT_H
#define SM_INIT_H

#include <Arduino.h>

#include "sm.h"

void sm_init_entry(sm_event_t last_event);

void sm_init_exit(void);

void sm_init_periodic(void);

#endif // SM_INIT_H
