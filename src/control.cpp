/**
 * @file control.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief main control loop
 * @date 2024-02-08
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include <stdlib.h>

#include "control.h"
#include "log.h"
#include "si_flow_sensor.h"
#include "si_relay.h"
#include "si_stepper.h"
#include "si_switch.h"
#include "sm.h"

#define FLOW_CORRECTION_FACTOR (2)

// limit switch variables
static bool my_sw_limit_max = false;
static bool my_sw_limit_min = false;

// pump variables
static int my_pump_trigger_off = 125;
static int my_pump_trigger_on = 250;
static bool pump_single_switch =
    rand() % 2; // Bool to remember which pump was last off

// flow sensor variables
static int flow_sensor_1_count = 0;
static int flow_sensor_2_count = 0;
static int flow_rate_1;
static int flow_rate_2;

// control variables
static bool my_sw_run = false;
static int my_setpoint = 0;
static int control_process_variable = 0;
static pump_enum_t pump_status = PUMPS_BOTH_OFF;

static int stepper_dir = 1;
static bool encoder_switch_history = false;

// function declarations
static void single_pump(void);
static void pump_status_check(void);
static void pump_control(void);

// tasks

/// @brief function that runs the control loop
/// @param
void control_task(void) {}

pump_enum_t pump_status_get(void) { return pump_status; }
int control_setpoint_get(void) { return my_setpoint; }
int control_process_variable_get(void) { return control_process_variable; }
bool limit_max_get(void) { return my_sw_limit_max; }
bool limit_min_get(void) { return my_sw_limit_min; }
bool control_active_get(void) { return my_sw_run; }

// setters
void control_setpoint_set(int new_control_setpoint) {
  my_setpoint = new_control_setpoint;
}

void control_process_variable_set(int new_process_variable) {
  control_process_variable = new_process_variable;
}

int flow_rate_calc(int flow_count) {
  return (flow_count * FLOW_CORRECTION_FACTOR);
}

void switch_task(void) {
  // checking the min limit switch
  if (si_switch_get(SW_LIMIT_MIN)) {
    my_sw_limit_min = true;
  } else {
    my_sw_limit_min = false;
  }

  // checking the max limit switch
  if (si_switch_get(SW_LIMIT_MAX)) {
    my_sw_limit_max = true;
  } else {
    my_sw_limit_max = false;
  }

  // check if the control switch is flipped
  if (si_switch_get(SW_RUN)) {
    my_sw_run = true;
  } else {
    my_sw_run = false;
  }

  pump_status_check();
  // controlling the water tunnel
  pump_control();
}

/// @brief logic to control which pump turns off when only one pump is needed
/// @param
static void single_pump(void) {
  // if the first pump was last off, set second to last off
  if (pump_single_switch) {
    pump_status = PUMPS_FIRST_ACTIVE;
    // if the second pump was the last off, set the first to last off
  } else {
    pump_status = PUMPS_SECOND_ACTIVE;
  }
  pump_single_switch = !pump_single_switch;
}

/// @brief checking the current status of the pumps and changing variables when
/// nessecary
/// @param
static void pump_status_check(void) {
  if (my_sw_run) {
    switch (pump_status) {
    case PUMPS_BOTH_OFF:
      if (my_setpoint > my_pump_trigger_on) {
        pump_status = PUMPS_BOTH_ACTIVE;
        LOG_INF("Both pumps on");
      } else {
        single_pump();
      }
      break;

    case PUMPS_BOTH_ACTIVE:
      if (my_setpoint < my_pump_trigger_off) {
        single_pump();
      }
      break;

    case PUMPS_FIRST_ACTIVE:
      if (my_setpoint > my_pump_trigger_on) {
        pump_status = PUMPS_BOTH_ACTIVE;
        LOG_INF("First Pump On");
      }
      break;

    case PUMPS_SECOND_ACTIVE:
      if (my_setpoint > my_pump_trigger_on) {
        pump_status = PUMPS_BOTH_ACTIVE;
      }
      break;

    default:
      LOG_ERR("unhandeled pump status");
      pump_status = PUMPS_BOTH_OFF;
    }
  } else {
    pump_status = PUMPS_BOTH_OFF;
  }
}

static void pump_control(void) {
  switch (pump_status) {
  case PUMPS_BOTH_OFF:
    si_relay_set(RELAY_PUMP_1, false);
    si_relay_set(RELAY_PUMP_2, false);
    break;

  case PUMPS_BOTH_ACTIVE:
    si_relay_set(RELAY_PUMP_1, true);
    si_relay_set(RELAY_PUMP_2, true);
    break;

  case PUMPS_FIRST_ACTIVE:
    si_relay_set(RELAY_PUMP_1, true);
    si_relay_set(RELAY_PUMP_2, false);
    break;

  case PUMPS_SECOND_ACTIVE:
    si_relay_set(RELAY_PUMP_1, false);
    si_relay_set(RELAY_PUMP_2, true);
    break;

  default:
    LOG_ERR("unhandeled pump event");
    break;
  }
}
