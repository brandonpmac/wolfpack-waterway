/**
 * @file sm.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief state machine definition
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>
#include <lstate.hpp>

#include "sm.h"
#include "sm_error.h"
#include "sm_idle.h"
#include "sm_init.h"
#include "sm_prime.h"
#include "sm_run.h"
#include "sm_shutdown.h"
#include "sm_types.h"

/// State Machine instance
static LState::StateMachine<sm_state_t, sm_event_t> *my_sm = nullptr;

/// State Machine state table
static LState::StateMachine<sm_state_t, sm_event_t>::sm_state_def_t
    my_state_table[] = {
        [SM_STATE_INIT] = {sm_init_entry, sm_init_exit, sm_init_periodic},
        [SM_STATE_IDLE] = {sm_idle_entry, sm_idle_exit, sm_idle_periodic},
        [SM_STATE_PRIME] = {sm_prime_entry, sm_prime_exit, sm_prime_periodic},
        [SM_STATE_RUN] = {sm_run_entry, sm_run_exit, sm_run_periodic},
        [SM_STATE_SHUTDOWN] = {sm_shutdown_entry, sm_shutdown_exit,
                               sm_shutdown_periodic},
        [SM_STATE_ERROR] = {sm_error_entry, sm_error_exit, sm_error_periodic},
};

/// State Machine transition table
static LState::StateMachine<sm_state_t, sm_event_t>::sm_transition_t
    my_transition_table[] = {
        {SM_STATE_INIT, SM_EVENT_INIT_COMPLETE, SM_STATE_IDLE}, // init -> idle
        {SM_STATE_IDLE, SM_EVENT_PRIME, SM_STATE_PRIME},        // idle -> prime
        {SM_STATE_PRIME, SM_EVENT_PRIME_COMPLETE, SM_STATE_RUN}, // prime -> run
        {SM_STATE_PRIME, SM_EVENT_PRIME_TO_SHUT,
         SM_STATE_SHUTDOWN}, // prime -> shutdown
        {SM_STATE_RUN, SM_EVENT_SHUTDOWN, SM_STATE_SHUTDOWN}, // run -> shutdown
        {SM_STATE_SHUTDOWN, SM_EVENT_SHUTDOWN_COMPLETE,
         SM_STATE_IDLE}, // shutdown -> idle
        {SM_STATE_SHUTDOWN, SM_EVENT_SHUT_TO_PRIME,
         SM_STATE_PRIME}, // shutdown -> prime
        {SM_STATE_INIT, SM_EVENT_ERROR_INIT, SM_STATE_ERROR}, // init -> error
        {SM_STATE_IDLE, SM_EVENT_ERROR_IDLE, SM_STATE_ERROR}, // idle -> error
        {SM_STATE_PRIME, SM_EVENT_ERROR_PRIME,
         SM_STATE_ERROR},                                   // prime -> error
        {SM_STATE_RUN, SM_EVENT_ERROR_RUN, SM_STATE_ERROR}, // run -> error
        {SM_STATE_SHUTDOWN, SM_EVENT_ERROR_SHUTDOWN,
         SM_STATE_ERROR}, // shutdown -> error

};
const size_t SM_TRANSITION_COUNT =
    sizeof(my_transition_table) / sizeof(my_transition_table[0]);

void sm_init(void) {
  my_sm = new LState::StateMachine<sm_state_t, sm_event_t>(
      my_state_table, (size_t)SM_STATE_COUNT, my_transition_table,
      SM_TRANSITION_COUNT, SM_STATE_INIT);
}

void sm_task(void) { my_sm->task(); }

bool sm_event_send(sm_event_t event) { return my_sm->send_event(event); }
