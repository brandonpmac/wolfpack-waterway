/**
 * @file sm_run.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "sm_run.h"
#include "sm_types.h"

void sm_run_entry(sm_event_t last_event) {
	Serial.println("Run entry");
}

void sm_run_exit(void) {
	Serial.println("Run exit");
}

void sm_run_periodic(void) {}