/**
 * @file sm_types.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SM_TYPES_H
#define SM_TYPES_H

typedef enum {
  SM_STATE_INIT,
  SM_STATE_IDLE,
  SM_STATE_RUN,
  SM_STATE_ERROR,

  SM_STATE_COUNT
} sm_state_t;

typedef enum {
  SM_EVENT_INIT_COMPLETE,
} sm_event_t;

#endif // SM_TYPES_H