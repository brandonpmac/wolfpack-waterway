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

#ifndef SM_IDLE_H
#define SM_IDLE_H

#include "sm_types.h"

void sm_idle_entry(sm_event_t last_event);

void sm_idle_exit(void);

void sm_idle_periodic(void);

#endif // SM_IDLE_H
