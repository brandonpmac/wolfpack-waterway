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
#include <EEPROM.h>
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

#define TUNNEL_MIN_SPEED (100)
#define TUNNEL_MAX_SPEED (500)

// function declarations
static uint32_t flow_rate_calc();
static void write_to_eeprom(uint16_t number, uint8_t address);
static uint16_t read_from_eeprom(uint8_t address);

// Variables ------------------------------------

// flow sensor variables
static uint32_t flow_sensor_1_count = 0;
static uint32_t flow_sensor_2_count = 0;
// static uint32_t flow_time = 0;
// static uint32_t flow_correction_factor = 220; // 21.5278208334

// switch variables
static bool my_sw_limit_max = false;
static bool my_sw_limit_min = false;
static bool my_sw_run = false;
static uint16_t encoder_increment = 5;

// control variables
static uint32_t my_tunnel_setpoint = 0;
static uint32_t my_tunnel_speed = 0;
static bool my_speed_override = false;
static uint16_t my_override_value = 0;

// PID Variables
static uint16_t my_p_const = read_from_eeprom(0);
static uint16_t my_i_const = read_from_eeprom(2);
static uint32_t i_term = 0;
static int32_t my_max_i = 50;
static int16_t my_min_i = -50;
static uint16_t my_d_const = read_from_eeprom(4);
static int32_t control_motor_speed = 0;
static float my_last_value = 0;

// tasks ----------------------------------

/// @brief encoder task stages an increase or decrease in the tunnel target
/// based on the rotation of the encoder
void encoder_task(void) {
  si_encoder_event_t event = si_encoder_event_get();
  switch (event) {
  case ENCODER_EVENT_NONE:
    break;

  case ENCODER_EVENT_LEFT:
    if (tunnel_setpoint_get() == 0) {
      break;
    }
    tunnel_setpoint_set(tunnel_setpoint_get() - encoder_increment);
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SETPOINT);
    break;

  case ENCODER_EVENT_RIGHT:
    if (tunnel_setpoint_get() == 1000) {
      break;
    }
    tunnel_setpoint_set(tunnel_setpoint_get() + encoder_increment);
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SETPOINT);
    break;

  default:
    LOG_ERR("unhandled encoder event");
    break;
  }
}

/// @brief Checks switches to see if any action needs to be taken
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

/// @brief flow sensor loop
void flow_sensor_task(void) {
  uint32_t new_tunnel_speed = flow_rate_calc();
  if (my_tunnel_speed != new_tunnel_speed) {
    my_tunnel_speed = new_tunnel_speed;
    display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SPEED);
  }
}

/// @brief pid loop
void control_task(void) {
  // PID value setup
  uint32_t error = my_tunnel_setpoint - my_tunnel_speed;

  // P Value
  uint32_t p_term = my_p_const * error;

  // I Value
  i_term = i_term + error;
  // Saturation check
  if (i_term > my_max_i) {
    i_term = my_max_i;
  } else if (i_term < my_min_i) {
    i_term = my_min_i;
  }
  i_term *= my_i_const;

  // D value
  uint32_t d_term = (my_tunnel_speed - my_last_value) * my_d_const;
  my_last_value = my_tunnel_speed;

  // PID Calculation
  // control_motor_speed = p_term + i_term - d_term;
  control_motor_speed = p_term + i_term - d_term;

  if ((control_motor_speed > 0) & si_switch_get(SW_LIMIT_MAX)) {
    control_motor_speed = 0;
  }

  if ((control_motor_speed < 0) & si_switch_get(SW_LIMIT_MIN)) {
    control_motor_speed = 0;
  }

  si_stepper_speed_set(control_motor_speed);
}

// functions ----------------------------

uint16_t tunnel_setpoint_get(void) { return my_tunnel_setpoint; }
uint32_t tunnel_speed_get(void) { return my_tunnel_speed; }

bool sw_limit_max_get(void) { return my_sw_limit_max; }
bool sw_limit_min_get(void) { return my_sw_limit_min; }
bool sw_run_get(void) { return my_sw_run; }

/// @brief sets the tunnel setpoint
/// @param new_setpoint new setpoint vale
void tunnel_setpoint_set(uint16_t new_setpoint) {
  my_tunnel_setpoint = new_setpoint;
  display_notification_send(DISPLAY_NOTIFICATION_TUNNEL_SETPOINT);
}

int16_t pid_values_get(int value) {
  switch (value) {
  case (0):
    return my_p_const;
    break;

  case (1):
    return my_i_const;
    break;

  case (2):
    return my_d_const;
    break;
  }
}

/// @brief calculates the current flow rate of the tunnel
/// @return flowspeed of the tonnel
static uint32_t flow_rate_calc() {
  flow_sensor_1_count = si_flow_sensor_rate_get(FLOW_SENSOR_1);
  flow_sensor_2_count = si_flow_sensor_rate_get(FLOW_SENSOR_2);
  uint32_t flow_count =
      (flow_sensor_1_count +
       flow_sensor_2_count); // amount of liters since last calc
  uint32_t flow_speed = (flow_count)*2;

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

/// @brief allows the shell script to set the pid values
/// @param pid specified value to change. 0 for p, 1 for i, 2 for d
/// @param new_value value to set the pid value to
void control_set_pid(int pid, uint16_t new_value) {
  switch (pid) {
  case 0:
    write_to_eeprom(new_value, 0);
    my_p_const = read_from_eeprom(0);
    break;

  case 1:
    write_to_eeprom(new_value, 2);
    my_i_const = read_from_eeprom(2);
    break;

  case 2:
    write_to_eeprom(new_value, 4);
    my_d_const = read_from_eeprom(4);
    break;
  }
}

static void write_to_eeprom(uint16_t number, uint8_t address) {
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;

  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}

static uint16_t read_from_eeprom(uint8_t address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);

  return (byte1 << 8) + byte2;
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
