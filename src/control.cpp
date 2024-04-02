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

#define FLOW_CORRECTION_FACTOR (22) // 21.5278208334
#define PUMP_TRIGGER_BOTH (250)
#define PUMP_TRIGGER_SINGLE (125)
#define PUMP_TRIGGER_OFF (5)
#define ENCODER_INCREMENT (5)
#define TUNNEL_MIN_SPEED (0)
#define TUNNEL_MAX_SPEED (1000)

// pump variables
static bool pump_single_switch =
    rand() % 2; // Bool to remember which pump was last off

// flow sensor variables
static int flow_sensor_1_count = 0;
static int flow_sensor_2_count = 0;

// switch variables
static bool my_sw_limit_max = false;
static bool my_sw_limit_min = false;
static bool my_sw_run = false;

// control variables
static int my_tunnel_setpoint = 0;
static int my_tunnel_speed = 0;
static pump_enum_t my_pump_status = PUMPS_NONE_ACTIVE;

// PID Variables

// function declarations
static pump_enum_t pump_check(pump_enum_t status);
static void pump_control(void);
static int flow_rate_calc(int);

// tasks
/// @brief control loop
void control_task(void) { static int32_t my_current_value = my_tunnel_speed; }

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
  }
  // checking the max limit switch
  if (si_switch_get(SW_LIMIT_MAX) != my_sw_limit_max) {
    display_notification_send(DISPLAY_NOTIFICATION_SW_LIMIT_MAX);
    my_sw_limit_max = !my_sw_limit_max;
  }
  // check if the control switch is flipped
  if (si_switch_get(SW_RUN) != my_sw_run) {
    my_sw_run = !my_sw_run;
  }

  static uint32_t new_tunnel_speed =
      flow_rate_calc(flow_sensor_1_count + flow_sensor_2_count);
  flow_sensor_1_count = 0;
  flow_sensor_2_count = 0;
  if (new_tunnel_speed != my_tunnel_speed) {
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SPEED);
    my_tunnel_speed = new_tunnel_speed;
  }
}

bool relay_pump_1_get(void) {
  switch (my_pump_status) {
  case PUMPS_BOTH_ACTIVE:
  case PUMPS_FIRST_ACTIVE:
    return true;
    break;

  case PUMPS_SECOND_ACTIVE:
  case PUMPS_NONE_ACTIVE:
    return false;
    break;

  case PUMPS_ERROR:
  default:
    return false;
    break;
  }
}

bool relay_pump_2_get(void) {
  switch (my_pump_status) {
  case PUMPS_BOTH_ACTIVE:
  case PUMPS_SECOND_ACTIVE:
    return true;
    break;

  case PUMPS_FIRST_ACTIVE:
  case PUMPS_NONE_ACTIVE:
    return false;
    break;

  case PUMPS_ERROR:
  default:
    return false;
    break;
  }
}

pump_enum_t pump_status_get(void) { return my_pump_status; }
uint16_t tunnel_setpoint_get(void) { return my_tunnel_setpoint; }
uint16_t tunnel_speed_get(void) { return my_tunnel_speed; }

bool sw_limit_max_get(void) { return my_sw_limit_max; }
bool sw_limit_min_get(void) { return my_sw_limit_min; }
bool sw_run_get(void) { return my_sw_run; }

void tunnel_setpoint_set(uint16_t new_setpoint) {
  my_tunnel_setpoint = new_setpoint;
}

int flow_rate_calc(int flow_count) {
  static uint32_t flow_time;
  uint32_t time = millis();
  return (flow_count * FLOW_CORRECTION_FACTOR);
}

static pump_enum_t pump_check(pump_enum_t status) {
  // Check to see if pumps should shut off
  if (!my_sw_run) {
    return PUMPS_NONE_ACTIVE;
  } else if (my_tunnel_setpoint <= PUMP_TRIGGER_OFF) {
    return PUMPS_NONE_ACTIVE;
  }

  switch (status) {
  case PUMPS_NONE_ACTIVE:
  case PUMPS_BOTH_ACTIVE:
    if (my_tunnel_setpoint <= PUMP_TRIGGER_SINGLE) {
      if (pump_single_switch) {
        pump_single_switch = !pump_single_switch;
        return PUMPS_FIRST_ACTIVE;
        // if the second pump was the last off, set the first to last off
      } else {
        pump_single_switch = !pump_single_switch;
        return PUMPS_SECOND_ACTIVE;
      }
    }
    return PUMPS_BOTH_ACTIVE;
    break;

  // Single pump mode check
  case PUMPS_FIRST_ACTIVE:
  case PUMPS_SECOND_ACTIVE:
    if (my_tunnel_setpoint >= PUMP_TRIGGER_BOTH) {
      return PUMPS_BOTH_ACTIVE;
    }
    return status;
    break;

  default:
    return PUMPS_NONE_ACTIVE;
  }
  LOG_ERR("Unhandled Pump Case")
  return PUMPS_ERROR;
}

static void pump_control(void) {
  switch (my_pump_status) {
  case PUMPS_NONE_ACTIVE:
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
