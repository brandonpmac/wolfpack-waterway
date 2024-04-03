/**
 * @file control.cpp
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief main control loop
 * @date 2024-02-08
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 */

#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>

#include "control.h"
#include "log.h"
#include "menu.h"
#include "si_encoder.h"
#include "si_flow_sensor.h"
#include "si_relay.h"
#include "si_stepper.h"
#include "si_switch.h"
#include "sm.h"

#define PUMP_TRIGGER_BOTH (250)
#define PUMP_TRIGGER_SINGLE (125)
#define PUMP_TRIGGER_OFF (5)
#define ENCODER_INCREMENT (5)
#define TUNNEL_MIN_SPEED (0)
#define TUNNEL_MAX_SPEED (500)

// pump variables
static bool pump_single_switch =
    rand() % 2; // Bool to remember which pump was last off

// flow sensor variables
static int flow_sensor_1_count = 0;
static int flow_sensor_2_count = 0;
// static uint32_t flow_time = 0;
// static uint32_t flow_correction_factor = 220; // 21.5278208334

// switch variables
static bool my_sw_limit_max = false;
static bool my_sw_limit_min = false;
static bool my_sw_run = false;

// control variables
static int32_t my_tunnel_setpoint = 0;
static int32_t my_tunnel_speed = 0;
static bool my_speed_override = false;
static uint16_t my_override_value = 0;

// PID Variables

static int16_t control_p = -10;
static int16_t control_i = 0;
static int16_t control_d = 0;
static int32_t control_motor_speed = 0;

// function declarations
static int32_t flow_rate_calc();

// tasks ----------------------------------

/// @brief flow sensor loop
void flow_sensor_task(void) {
  static int32_t new_tunnel_speed = flow_rate_calc();
  if (my_tunnel_speed != new_tunnel_speed) {
    my_tunnel_speed = new_tunnel_speed;
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SPEED);
  }
}

void control_task(void) {
  int32_t error = my_tunnel_setpoint - my_tunnel_speed;
  int32_t p_correction = control_p * error;
  control_motor_speed = p_correction;

  if (control_motor_speed > 0 & si_switch_get(SW_LIMIT_MAX)) {
    control_motor_speed = 0;
  }

  if (control_motor_speed < 0 & si_switch_get(SW_LIMIT_MIN)) {
    control_motor_speed = 0;
  }

  si_stepper_speed_set(control_motor_speed);
}

void encoder_task(void) {
  si_encoder_event_t event = si_encoder_event_get();
  switch (event) {
  case ENCODER_EVENT_NONE:
    break;

  case ENCODER_EVENT_LEFT:
    if (tunnel_setpoint_get() == 0) {
      break;
    }
    tunnel_setpoint_set(tunnel_setpoint_get() - ENCODER_INCREMENT);
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SETPOINT);
    break;

  case ENCODER_EVENT_RIGHT:
    if (tunnel_setpoint_get() == 1000) {
      break;
    }
    tunnel_setpoint_set(tunnel_setpoint_get() + ENCODER_INCREMENT);
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SETPOINT);
    break;

  default:
    LOG_ERR("unhandled encoder event");
    break;
  }
}

void switch_task(void) {
  // checking the min limit switch
  if (si_switch_get(SW_LIMIT_MIN) != my_sw_limit_min) {
    display_notification_send(DISPLAY_NOTIFICATION_SW_LIMIT_MIN);
    my_sw_limit_min = !my_sw_limit_min;
    if (my_sw_limit_min & (control_motor_speed < 0)) {
      si_stepper_speed_set(0);
    }
  }
  // checking the max limit switch
  if (si_switch_get(SW_LIMIT_MAX) != my_sw_limit_max) {
    display_notification_send(DISPLAY_NOTIFICATION_SW_LIMIT_MAX);
    my_sw_limit_max = !my_sw_limit_max;
    if (my_sw_limit_max & (control_motor_speed > 0)) {
      si_stepper_speed_set(0);
    }
  }
  // check if the control switch is flipped
  if (si_switch_get(SW_RUN) != my_sw_run) {
    my_sw_run = !my_sw_run;
  }
}

// functions ----------------------------

int32_t tunnel_setpoint_get(void) { return my_tunnel_setpoint; }
int32_t tunnel_speed_get(void) { return my_tunnel_speed; }

bool sw_limit_max_get(void) { return my_sw_limit_max; }
bool sw_limit_min_get(void) { return my_sw_limit_min; }
bool sw_run_get(void) { return my_sw_run; }

void tunnel_setpoint_set(int32_t new_setpoint) {
  my_tunnel_setpoint = new_setpoint;
}

static int32_t flow_rate_calc() {
  flow_sensor_1_count = si_flow_sensor_rate_get(FLOW_SENSOR_1);
  flow_sensor_2_count = si_flow_sensor_rate_get(FLOW_SENSOR_2);
  int32_t flow_count =
      (flow_sensor_1_count +
       flow_sensor_2_count); // amount of half liters since last calc
  int32_t flow_speed = (flow_count);

  // override
  if (my_speed_override) {
    flow_speed = my_override_value;
  }
  return flow_speed;
}

/// @brief allows the shell script to override the actual tunnel speed for
/// testing purposes
/// @param override bool 1 or 0, 1 activates the override
/// @param new_speed new speed for the tunnel
void tunnel_speed_set(bool override, int32_t new_speed) {
  if (override) {
    my_speed_override = true;
    my_override_value = new_speed;
  } else {
    my_speed_override = false;
  }
}

void control_set_pid(int pid, uint16_t new_value) {
  switch (pid) {
  case 0:
    control_p = new_value;
    break;

  case 1:
    control_i = new_value;
    break;

  case 2:
    control_d = new_value;
    break;
  }
}

#ifdef TEST_BUILD

#include "../test/mock/mock_si_switch.h"
#include <gtest/gtest.h>

void pump_status_set(pump_enum_t status) { my_pump_status = status; }

// Pump Status Check
TEST(CTRL_PumpStatusCheck, both_above_trigger_both) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_below_trigger_both) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH - 1;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_to_off) {
  // Setup
  pump_enum_t result;
  my_sw_run = false;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_below_trigger_off) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_OFF;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_to_first) {
  // Setup
  pump_enum_t result;

  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_SINGLE - 1;
  pump_single_switch = true;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_FIRST_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, both_to_second) {
  // Setup
  pump_enum_t result;

  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_SINGLE - 1;
  pump_single_switch = false;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_SECOND_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, first_to_both) {
  // Setup
  pump_enum_t result;

  pump_status_set(PUMPS_FIRST_ACTIVE);
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_FIRST_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, first_to_first) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH - 1;

  // Act
  result = pump_check(PUMPS_FIRST_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_FIRST_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, first_to_off) {
  // Setup
  pump_enum_t result;
  my_sw_run = false;

  // Act
  result = pump_check(PUMPS_FIRST_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, off_to_both) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, off_to_first) {
  // Setup
  pump_enum_t result;

  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_SINGLE - 1;
  pump_single_switch = true;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_FIRST_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, off_to_off) {
  // Setup
  pump_enum_t result;
  my_sw_run = false;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, off_to_second) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_SINGLE - 1;
  pump_single_switch = false;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_SECOND_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, second_to_both) {
  // Setup
  pump_enum_t result;

  pump_status_set(PUMPS_SECOND_ACTIVE);
  my_sw_run = true;
  my_tunnel_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_SECOND_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}
TEST(CTRL_PumpStatusCheck, second_to_off) {
  // Setup
  pump_enum_t result;

  pump_status_set(PUMPS_SECOND_ACTIVE);
  my_sw_run = false;

  // Act
  result = pump_check(PUMPS_SECOND_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}

// SW_MIN tests
TEST(SW_TASK, min_low_high) {
  // Setup
  bool result;
  my_sw_limit_min = false;
  si_switch_mock_set_value(SW_LIMIT_MIN, true);
  // Act
  switch_task();
  result = my_sw_limit_min;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, min_low_low) {
  // Setup
  bool result;
  my_sw_limit_min = false;
  si_switch_mock_set_value(SW_LIMIT_MIN, false);
  // Act
  switch_task();
  result = my_sw_limit_min;

  // Verify
  ASSERT_EQ(result, false);
}
TEST(SW_TASK, min_high_high) {
  // Setup
  bool result;
  my_sw_limit_min = true;
  si_switch_mock_set_value(SW_LIMIT_MIN, true);
  // Act
  switch_task();
  result = my_sw_limit_min;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, min_high_low) {
  // Setup
  bool result;
  my_sw_limit_min = true;
  si_switch_mock_set_value(SW_LIMIT_MIN, false);
  // Act
  switch_task();
  result = my_sw_limit_min;

  // Verify
  ASSERT_EQ(result, false);
}

// SW_MAX tests
TEST(SW_TASK, max_low_high) {
  // Setup
  bool result;
  my_sw_limit_max = false;
  si_switch_mock_set_value(SW_LIMIT_MAX, true);
  // Act
  switch_task();
  result = my_sw_limit_max;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, max_low_low) {
  // Setup
  bool result;
  my_sw_limit_max = false;
  si_switch_mock_set_value(SW_LIMIT_MAX, false);
  // Act
  switch_task();
  result = my_sw_limit_max;

  // Verify
  ASSERT_EQ(result, false);
}
TEST(SW_TASK, max_high_high) {
  // Setup
  bool result;
  my_sw_limit_max = true;
  si_switch_mock_set_value(SW_LIMIT_MAX, true);
  // Act
  switch_task();
  result = my_sw_limit_max;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, max_high_low) {
  // Setup
  bool result;
  my_sw_limit_max = true;
  si_switch_mock_set_value(SW_LIMIT_MAX, false);
  // Act
  switch_task();
  result = my_sw_limit_max;

  // Verify
  ASSERT_EQ(result, false);
}

// SW_RUN tests
TEST(SW_TASK, run_low_high) {
  // Setup
  bool result;
  my_sw_run = false;
  si_switch_mock_set_value(SW_RUN, true);
  // Act
  switch_task();
  result = my_sw_run;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, run_low_low) {
  // Setup
  bool result;
  my_sw_limit_min = false;
  si_switch_mock_set_value(SW_RUN, false);
  // Act
  switch_task();
  result = my_sw_run;

  // Verify
  ASSERT_EQ(result, false);
}
TEST(SW_TASK, run_high_high) {
  // Setup
  bool result;
  my_sw_limit_min = true;
  si_switch_mock_set_value(SW_RUN, true);
  // Act
  switch_task();
  result = my_sw_run;

  // Verify
  ASSERT_EQ(result, true);
}
TEST(SW_TASK, run_high_low) {
  // Setup
  bool result;
  my_sw_run = true;
  si_switch_mock_set_value(SW_RUN, false);
  // Act
  switch_task();
  result = my_sw_run;

  // Verify
  ASSERT_EQ(result, false);
}
#endif // TEST_BUILD
