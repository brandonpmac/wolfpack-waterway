/**
 * @file sm_shutdown.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-04-02
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SM_SHUTDOWN_H
#define SM_SHUTDOWN_H

#include "sm_types.h"

typedef enum {
  SHUTDOWN_ENTRY,
  SHUTDOWN_FIRST,
  SHUTDOWN_SECOND,
  SHUTDOWN_COMPLETE,

  SHUTDOWN_CNT,
  SHUTDOWN_ERR = SHUTDOWN_CNT,
} sm_shutdown_state_t;

void sm_shutdown_entry(sm_event_t last_event);

void sm_shutdown_exit(void);

void sm_shutdown_periodic(void);

#endif // SM_IDLE_H