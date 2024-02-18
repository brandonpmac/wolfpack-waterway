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

#include <lstate.hpp>

typedef LState::StateMachine::sm_state_t sm_state_t;
enum {
  SM_STATE_INIT = 0,
  SM_STATE_IDLE,
  SM_STATE_RUN,
  SM_STATE_ERROR,

  SM_STATE_COUNT
};

typedef LState::StateMachine::sm_event_t sm_event_t;
enum {
  SM_EVENT_INIT_COMPLETE,
};

#endif // SM_TYPES_H
