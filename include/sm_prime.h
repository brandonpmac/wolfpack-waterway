/**
 * @file sm_prime.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-04-02
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SM_PRIME_H
#define SM_PRIME_H

#include "sm_types.h"

typedef enum {
  PRIME_ENTRY,
  PRIME_FIRST,
  PRIME_SECOND,
  PRIME_COMPLETE,
  PRIME_SHUTDOWN,

  PRIME_CNT,
  PRIME_ERR = PRIME_CNT,
} sm_prime_state_t;

void sm_prime_entry(sm_event_t last_event);

void sm_prime_exit(void);

void sm_prime_periodic(void);

#endif // SM_PRIME_H