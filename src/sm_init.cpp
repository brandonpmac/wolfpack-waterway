/**
 * @file sm_init.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-02-18
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#include <Arduino.h>

#include "sm_init.h"
#include "sm_types.h"

void sm_init_entry(sm_event_t last_event) {
	Serial.println("Init entry");
}

void sm_init_exit(void) {
	Serial.println("Init exit");
}

void sm_init_periodic(void) {}