/**
 * @file control.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief main control loop
 * @date 2024-02-08
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include "control.h"
#include "init.h"
#include "menu.h"
#include "pins.h"

// limit switch variables
static bool limit_max = false;
static bool limit_min = false;

// pump variables
static int pump_trigger_off = 90;
static int pump_trigger_on = 120;
static bool pump_single_switch =
    random(2); // Bool to remember which pump was last off

// flow sensor variables
static int flow_sensor_1_count = 0;
static int flow_sensor_2_count = 0;
static int flow_correction_factor = 0.5;
static int flow_rate_1;
static int flow_rate_2;

// control variables
static bool control_active = false;
static int control_setpoint = 0;
static int control_process_variable = 0;
static pump_enum_t pump_status = PUMPS_BOTH_OFF;

// function declarations
static void switch_check(void);
static void single_pump(void);
static void pump_status_check(void);
static void pump_control(void);

// tasks

/// @brief function that runs the control loop
/// @param
void control_task(void) {
  // checking status and performing logic
  switch_check();
  pump_status_check();

  // controlling the water tunnel
  pump_control();
}

// getters
pump_enum_t pump_status_get(void) { return pump_status; }
int control_setpoint_get(void) { return control_setpoint; }
int control_process_variable_get(void) { return control_process_variable; }
bool limit_max_get(void) { return limit_max; }
bool limit_min_get(void) { return limit_min; }
bool control_active_get(void) { return control_active; }

// setters
void control_setpoint_set(int new_control_setpoint) {
  control_setpoint = new_control_setpoint;
}

void ISR_flow_sensor_1_SIGNAL(void) { flow_sensor_1_count += 1; }

void ISR_flow_sensor_2_SIGNAL(void) { flow_sensor_2_count += 1; }

void flow_rate_calc(void) {
  flow_rate_1 = (flow_correction_factor * flow_sensor_1_count) / 60;
  flow_rate_2 = (flow_correction_factor * flow_sensor_2_count) / 60;
  flow_sensor_1_count = 0;
  flow_sensor_2_count = 0;
}

static void switch_check(void) {
  // checking the max limit switch
  if (digitalRead(SW_LIMIT_MAX) == LOW) {
    limit_max = true;
    Serial.println("LIMIT_MAX");
  } else {
    limit_max = false;
  }
  // checking the min limit switch
  if (digitalRead(SW_LIMIT_MIN) == LOW) {
    limit_min = true;
    Serial.println("LIMIT_MIN");
  } else {
    limit_min = false;
  }
  // check if the control switch is flipped
  if (digitalRead(SW_RUN) == LOW) {
    control_active = true;
  } else {
    control_active = false;
  }
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
  if (control_active) {
    switch (pump_status) {
    case PUMPS_BOTH_OFF:
      if (control_setpoint > pump_trigger_on) {
        pump_status = PUMPS_BOTH_ACTIVE;
      } else {
        single_pump();
      }
      break;

    case PUMPS_BOTH_ACTIVE:
      if (control_setpoint < pump_trigger_off) {
        single_pump();
      }
      break;

    case PUMPS_FIRST_ACTIVE:
      if (control_setpoint > pump_trigger_on) {
        pump_status = PUMPS_BOTH_ACTIVE;
      }
      break;

    case PUMPS_SECOND_ACTIVE:
      if (control_setpoint > pump_trigger_on) {
        pump_status = PUMPS_BOTH_ACTIVE;
      }
      break;

    default:
      Serial.println("unhandeled pump status");
      pump_status = PUMPS_BOTH_OFF;
    }
  } else {
    pump_status = PUMPS_BOTH_OFF;
  }
}

static void pump_control(void) {
  switch (pump_status) {
  case PUMPS_BOTH_OFF:
    digitalWrite(PUMP_RELAY_1, LOW);
    digitalWrite(PUMP_RELAY_2, LOW);
    break;

  case PUMPS_BOTH_ACTIVE:
    digitalWrite(PUMP_RELAY_1, HIGH);
    digitalWrite(PUMP_RELAY_2, HIGH);
    break;

  case PUMPS_FIRST_ACTIVE:
    digitalWrite(PUMP_RELAY_1, HIGH);
    digitalWrite(PUMP_RELAY_2, LOW);
    break;

  case PUMPS_SECOND_ACTIVE:
    digitalWrite(PUMP_RELAY_1, HIGH);
    digitalWrite(PUMP_RELAY_2, LOW);
    break;

  default:
    Serial.println("unhandeled pump event");
    break;
  }
}
