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
#define PUMP_TRIGGER_BOTH (250)
#define PUMP_TRIGGER_SINGLE (125)
#define PUMP_TRIGGER_OFF (5)

// limit switch variables
static bool my_sw_limit_max = false;
static bool my_sw_limit_min = false;
static bool my_sw_run = false;

// pump variables
static bool pump_single_switch =
    rand() % 2; // Bool to remember which pump was last off

// flow sensor variables
static int flow_sensor_1_count = 0;
static int flow_sensor_2_count = 0;

// control variables
static int my_setpoint = 0;
static int my_tunnel_speed = 0;
static pump_enum_t pump_status = PUMPS_NONE_ACTIVE;

// function declarations
static pump_enum_t pump_check(pump_enum_t status);
static void pump_control(void);

// tasks
/// @brief control loop
void control_task(void) {}

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
}

pump_enum_t pump_status_get(void) { return pump_status; }
int control_setpoint_get(void) { return my_setpoint; }
int control_tunnel_speed_get(void) { return my_tunnel_speed; }
bool limit_max_get(void) { return my_sw_limit_max; }
bool limit_min_get(void) { return my_sw_limit_min; }
bool control_active_get(void) { return my_sw_run; }

void control_tunnel_setpoint_set(int new_setpoint) {
  my_setpoint = new_setpoint;
}

int flow_rate_calc(int flow_count) {
  return (flow_count * FLOW_CORRECTION_FACTOR);
}

static pump_enum_t pump_check(pump_enum_t status) {
  // Check to see if pumps should shut off
  if (!my_sw_run) {
    return PUMPS_NONE_ACTIVE;
  } else if (my_setpoint <= PUMP_TRIGGER_OFF) {
    return PUMPS_NONE_ACTIVE;
  }

  switch (status) {
  case PUMPS_NONE_ACTIVE:
  case PUMPS_BOTH_ACTIVE:
    if (my_setpoint <= PUMP_TRIGGER_SINGLE) {
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
    if (my_setpoint >= PUMP_TRIGGER_BOTH) {
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
  switch (pump_status) {
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

void pump_status_set(pump_enum_t status) { pump_status = status; }

// Pump Status Check ------------------------------------------------

TEST(CTRL_PumpStatusCheck, both_above_trigger_both) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}

TEST(CTRL_PumpStatusCheck, both_below_trigger_both) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_setpoint = PUMP_TRIGGER_BOTH - 1;

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
  my_setpoint = PUMP_TRIGGER_OFF;

  // Act
  result = pump_check(PUMPS_BOTH_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_NONE_ACTIVE);
}

TEST(CTRL_PumpStatusCheck, both_to_first) {
  // Setup
  pump_enum_t result;

  my_sw_run = true;
  my_setpoint = PUMP_TRIGGER_SINGLE - 1;
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
  my_setpoint = PUMP_TRIGGER_SINGLE - 1;
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
  my_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_FIRST_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}

TEST(CTRL_PumpStatusCheck, first_to_first) {
  // Setup
  pump_enum_t result;
  my_sw_run = true;
  my_setpoint = PUMP_TRIGGER_BOTH - 1;

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
  my_setpoint = PUMP_TRIGGER_BOTH + 1;

  // Act
  result = pump_check(PUMPS_NONE_ACTIVE);

  // Verify
  ASSERT_EQ(result, PUMPS_BOTH_ACTIVE);
}

TEST(CTRL_PumpStatusCheck, off_to_first) {
  // Setup
  pump_enum_t result;

  my_sw_run = true;
  my_setpoint = PUMP_TRIGGER_SINGLE - 1;
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
  my_setpoint = PUMP_TRIGGER_SINGLE - 1;
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
  my_setpoint = PUMP_TRIGGER_BOTH + 1;

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

#endif // TEST_BUILD
