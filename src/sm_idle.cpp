/**
 * @file sm_idle.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "sm_idle.h"
#include "sm_types.h"

void sm_idle_entry(sm_event_t last_event) {
	Serial.println("Idle entry");
}

void sm_idle_exit(void) {
	Serial.println("Idle exit");
}

void sm_idle_periodic(void) {}