/**
 * @file sm_error.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SM_ERROR_H
#define SM_ERROR_H

#include "sm_types.h"

void sm_error_entry(sm_event_t last_event);

void sm_error_exit(void);

void sm_error_periodic(void);

#endif // SM_ERROR_H
