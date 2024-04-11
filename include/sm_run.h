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

#include "sm_types.h"

typedef enum {
  RUN_NORMAL,
  RUN_TEST,
  RUN_TEST_DELAY,
  RUN_TEST_START,
  RUN_TEST_END,

  RUN_CNT,
  RUN_ERROR = RUN_CNT,
} sm_run_state_t;

void sm_run_entry(sm_event_t last_event);

void sm_run_exit(void);

void sm_run_periodic(void);

void sm_run_test(uint16_t speed, uint32_t time);

#endif // SM_RUN_H
