/**
 * @file sm.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SM_H
#define SM_H

#include "sm_types.h"

void sm_init(void);

void sm_task(void);

bool sm_event_send(sm_event_t event);

#endif // SM_H
